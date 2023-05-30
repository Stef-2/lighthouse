#include "lighthouse/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/vulkan/logical_device.hpp"

lh::vulkan::descriptor_set_layout::descriptor_set_layout(const logical_device& logical_device,
														 const std::vector<binding>& bindings,
														 const create_info& create_info)
	: m_bindings {bindings}
{
	auto layout_bindings = std::vector<vk::DescriptorSetLayoutBinding> {bindings.size()};

	for (const auto& binding : bindings)
		layout_bindings.emplace_back(binding.m_location, binding.m_type, binding.m_count, create_info.m_access);

	const auto layout_info = vk::DescriptorSetLayoutCreateInfo(create_info.m_flags, layout_bindings);

	m_object = {*logical_device, layout_info};
}

auto lh::vulkan::descriptor_set_layout::bindings() const -> const std::vector<binding>&
{
	return m_bindings;
}
