module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <ranges>
#endif

module descriptor_set_layout;

namespace lh
{
	namespace vulkan
	{

		descriptor_set_layout::descriptor_set_layout(const logical_device& logical_device,
													 const std::vector<binding>& bindings,
													 const create_info& create_info)
			: m_bindings {}
		{
			auto layout_bindings = std::vector<vk::DescriptorSetLayoutBinding> {};
			layout_bindings.reserve(bindings.size());

			for (const auto& binding : bindings)
				layout_bindings.emplace_back(binding.m_binding, binding.m_type, binding.m_count, create_info.m_access);

			const auto layout_info = vk::DescriptorSetLayoutCreateInfo(create_info.m_modifier_flags, layout_bindings);

			m_object = {*logical_device, layout_info};
			m_bindings = bindings;
		}

		auto descriptor_set_layout::storage_descriptors() const -> const std::vector<const binding*>
		{
			auto storage_descriptors = std::vector<const binding*> {};

			for (const auto& binding : m_bindings)
				if (binding.m_type == vk::DescriptorType::eUniformBuffer or
					binding.m_type == vk::DescriptorType::eStorageBuffer or
					binding.m_type == vk::DescriptorType::eUniformBufferDynamic or
					binding.m_type == vk::DescriptorType::eStorageBufferDynamic)
					storage_descriptors.push_back(&binding);

			return storage_descriptors;
		}

		auto descriptor_set_layout::image_sampler_descriptors() const -> const std::vector<const binding*>
		{
			auto image_sampler_descriptors = std::vector<const binding*> {};

			for (const auto& binding : m_bindings)
				if (binding.m_type == vk::DescriptorType::eSampler or
					binding.m_type == vk::DescriptorType::eSampledImage or
					binding.m_type == vk::DescriptorType::eCombinedImageSampler)
					image_sampler_descriptors.push_back(&binding);

			return image_sampler_descriptors;
		}

		auto descriptor_set_layout::bindings() const -> const std::vector<binding>&
		{
			return m_bindings;
		}
	}
}
