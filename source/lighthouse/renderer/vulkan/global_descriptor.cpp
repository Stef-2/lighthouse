module;

#if INTELLISENSE
	#include "vulkan/vulkan_raii.hpp"

	#include <numeric>
#endif

module global_descriptor;

namespace lh
{
	namespace vulkan
	{
		global_descriptor::global_descriptor(const physical_device& physical_device,
											 const logical_device& logical_device,
											 const create_info& create_info)
			: m_create_info {create_info},
			  m_uniform_buffer_set {nullptr},
			  m_storage_buffer_set {nullptr},
			  m_combined_image_sampler_set {nullptr},
			  m_push_constant_range {vk::ShaderStageFlagBits::eAll,
									 0,
									 physical_device.properties().m_properties.properties.limits.maxPushConstantsSize},
			  m_pipeline_layout {nullptr}
		{
			const auto& physical_device_properties = physical_device.properties();

			auto descriptor_set_layout_usage = vk::DescriptorSetLayoutCreateFlags {
				vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT};

			// uniform buffers
			m_create_info.m_num_uniform_buffers = std::min(
				{physical_device_properties.m_descriptor_indexing_properties
					 .maxPerStageDescriptorUpdateAfterBindUniformBuffers,
				 physical_device_properties.m_properties.properties.limits.maxDescriptorSetUniformBuffers,
				 m_create_info.m_num_uniform_buffers});
			auto bindings = std::vector<vk::DescriptorSetLayoutBinding> {};
			bindings.reserve(m_create_info.m_num_uniform_buffers);

			for (auto i = descriptor_type_size_t {}; i < m_create_info.m_num_uniform_buffers; i++)
				bindings.emplace_back(i, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAll);

			m_uniform_buffer_set = {vk::raii::DescriptorSetLayout {
				*logical_device, vk::DescriptorSetLayoutCreateInfo {descriptor_set_layout_usage, bindings}}};

			// storage descriptors
			m_create_info.m_num_storage_buffers = std::min(
				{physical_device_properties.m_descriptor_buffer_properties.m_properties.maxDescriptorBufferBindings,
				 physical_device_properties.m_descriptor_indexing_properties
					 .maxPerStageDescriptorUpdateAfterBindStorageBuffers,
				 m_create_info.m_num_storage_buffers});
			bindings = std::vector<vk::DescriptorSetLayoutBinding> {};
			bindings.reserve(m_create_info.m_num_storage_buffers);

			for (auto i = descriptor_type_size_t {}; i < m_create_info.m_num_storage_buffers; i++)
				bindings.emplace_back(i, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eAll);

			m_storage_buffer_set = {
				vk::raii::DescriptorSetLayout {*logical_device, {descriptor_set_layout_usage, bindings}}};

			// combined image samplers
			m_create_info.m_num_combined_image_samplers = std::min(
				{physical_device_properties.m_descriptor_indexing_properties
					 .maxDescriptorSetUpdateAfterBindSampledImages,
				 create_info.m_num_combined_image_samplers});

			auto descriptor_indexing_flags = vk::DescriptorBindingFlags {
				vk::DescriptorBindingFlagBits::eVariableDescriptorCount};
			auto descriptor_indexing_bindings = vk::DescriptorSetLayoutBindingFlagsCreateInfo {
				descriptor_indexing_flags};

			const auto combined_image_sampler_binding = vk::DescriptorSetLayoutBinding {
				0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eAll};

			m_combined_image_sampler_set =
				vk::raii::DescriptorSetLayout {*logical_device,
											   vk::DescriptorSetLayoutCreateInfo {descriptor_set_layout_usage,
																				  combined_image_sampler_binding,
																				  &descriptor_indexing_bindings}};

			// pipeline layout
			auto descriptor_sets = std::array<vk::DescriptorSetLayout, 3> {*m_uniform_buffer_set,
																		   *m_storage_buffer_set,
																		   *m_combined_image_sampler_set};
			m_pipeline_layout = {*logical_device, {{}, descriptor_sets, {m_push_constant_range}}};
		}

		auto global_descriptor::uniform_buffer_set() const -> const vk::raii::DescriptorSetLayout&
		{
			return m_uniform_buffer_set;
		}

		auto global_descriptor::storage_buffer_set() const -> const vk::raii::DescriptorSetLayout&
		{
			return m_storage_buffer_set;
		}

		auto global_descriptor::combined_image_sampler_set() const -> const vk::raii::DescriptorSetLayout&
		{
			return m_combined_image_sampler_set;
		}

		auto global_descriptor::descriptor_set_layouts() const -> const std::array<vk::DescriptorSetLayout, 3>
		{
			return {*m_uniform_buffer_set, *m_storage_buffer_set, *m_combined_image_sampler_set};
		}

		auto global_descriptor::push_constant_range() const -> const vk::PushConstantRange&
		{
			return m_push_constant_range;
		}

		auto global_descriptor::pipeline_layout() const -> const vk::raii::PipelineLayout&
		{
			return m_pipeline_layout;
		}

		auto global_descriptor::create_information() const -> const create_info&
		{
			return m_create_info;
		}
	}
}
