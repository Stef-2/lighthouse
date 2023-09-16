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
		global_descriptor::global_descriptor(const logical_device& logical_device, const create_info& create_info)
			: m_immutable_sampler {*logical_device, create_info.m_immutable_sampler_properties},
			  m_uniform_buffer_set {nullptr},
			  m_storage_descriptor_set {nullptr},
			  m_combined_image_sampler_set {nullptr},
			  m_pipeline_layout {nullptr}
		{
			auto descriptor_set_layout_usage = vk::DescriptorSetLayoutCreateFlags {
				vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT/* |
				vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool*/};

			// uniform buffers
			auto bindings = std::vector<vk::DescriptorSetLayoutBinding> {};
			bindings.reserve(create_info.m_num_uniform_buffers);

			for (auto i = create_info::descriptor_type_size_t {}; i < create_info.m_num_uniform_buffers; i++)
				bindings.emplace_back(i, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAll);

			m_uniform_buffer_set = {
				vk::raii::DescriptorSetLayout {*logical_device, {descriptor_set_layout_usage, bindings}}};

			// storage descriptors
			bindings = std::vector<vk::DescriptorSetLayoutBinding> {};
			bindings.reserve(create_info.m_num_storage_descriptors);

			for (auto i = create_info::descriptor_type_size_t {}; i < create_info.m_num_storage_descriptors; i++)
				bindings.emplace_back(i, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eAll);

			m_storage_descriptor_set = {
				vk::raii::DescriptorSetLayout {*logical_device, {descriptor_set_layout_usage, bindings}}};

			// combined image samplers
			bindings = std::vector<vk::DescriptorSetLayoutBinding> {};
			bindings.reserve(create_info.m_num_storage_descriptors);

			for (auto i = create_info::descriptor_type_size_t {}; i < create_info.m_num_storage_descriptors; i++)
				bindings.emplace_back(
					i, vk::DescriptorType::eSampler, 1, vk::ShaderStageFlagBits::eAll, &*m_immutable_sampler);

			m_combined_image_sampler_set = {vk::raii::DescriptorSetLayout {
				*logical_device,
				{descriptor_set_layout_usage | vk::DescriptorSetLayoutCreateFlagBits::eEmbeddedImmutableSamplersEXT,
				 bindings}}};

			// pipeline layout
			auto descriptor_sets = std::array<vk::DescriptorSetLayout, 3> {*m_uniform_buffer_set,
																		   *m_storage_descriptor_set,
																		   *m_combined_image_sampler_set};
			m_pipeline_layout = {*logical_device, {{}, descriptor_sets}};
		}

		auto global_descriptor::uniform_buffer_set() const -> const vk::raii::DescriptorSetLayout&
		{
			return m_uniform_buffer_set;
		}

		auto global_descriptor::storage_descriptor_set() const -> const vk::raii::DescriptorSetLayout&
		{
			return m_storage_descriptor_set;
		}

		auto global_descriptor::combined_image_sampler_set() const -> const vk::raii::DescriptorSetLayout&
		{
			return m_combined_image_sampler_set;
		}

		auto global_descriptor::pipeline_layout() const -> const vk::raii::PipelineLayout&
		{
			return m_pipeline_layout;
		}
	}
}
