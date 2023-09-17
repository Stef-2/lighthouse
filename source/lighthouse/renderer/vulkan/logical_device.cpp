module;

#if INTELLISENSE
#include "vulkan.hpp"
#endif

module logical_device;

import output;

namespace lh
{
	namespace vulkan
	{

		logical_device::logical_device(const physical_device& physical_device, const create_info& create_info)
		{
			const auto& suported_extensions = physical_device.extensions().supported_extensions();

			if (not physical_device.extensions().assert_required_extensions())
				output::error() << "this system does not support the required vulkan components";

			auto dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeatures {true};
			auto buffer_addressing =
				vk::PhysicalDeviceBufferDeviceAddressFeatures {true, false, false, &dynamic_rendering};
			auto descriptor_buffering =
				vk::PhysicalDeviceDescriptorBufferFeaturesEXT {true, false, false, true, &buffer_addressing};
			auto shader_object = vk::PhysicalDeviceShaderObjectFeaturesEXT {true, &descriptor_buffering};
			auto vertex_input = vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT {true, &shader_object};
			auto synchronization2 = vk::PhysicalDeviceSynchronization2Features {true, &vertex_input};
			auto host_image_copy = vk::PhysicalDeviceHostImageCopyFeaturesEXT {true, &synchronization2};
			auto maintenantce_5 = vk::PhysicalDeviceMaintenance5FeaturesKHR {true, &host_image_copy};

			auto features = create_info.m_features;

			features.pNext = &maintenantce_5;

			auto device_info = vk::DeviceCreateInfo {
				{}, create_info.m_queues, {}, create_info.m_extensions, &features.features, &maintenantce_5};

			m_object = {*physical_device, device_info};
		}

		auto logical_device::info() const -> lh::string::string_t
		{
			return {};
		}
	}
}
