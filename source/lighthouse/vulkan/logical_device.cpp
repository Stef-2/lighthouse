#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/physical_device.hpp"

lh::vulkan::logical_device::logical_device(const physical_device& physical_device, const create_info& create_info)
{
	const auto& suported_extensions = physical_device.extensions().supported_extensions();

	if (not physical_device.extensions().assert_required_extensions())
		output::error() << "this system does not support the required vulkan components";

	const auto dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeatures {true};

	auto device_info = vk::DeviceCreateInfo {
		{}, create_info.m_queues, {}, create_info.m_extensions, &create_info.m_features.features, &dynamic_rendering};

	m_object = {*physical_device, device_info};
}

auto lh::vulkan::logical_device::info() const -> lh::string::string_t
{
	return {};
}
