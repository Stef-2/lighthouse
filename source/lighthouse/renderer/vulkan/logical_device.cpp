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
			const auto& suported_features = physical_device.features().m_features;
			const auto null_features = vk::PhysicalDeviceFeatures {};

			if (not physical_device.extensions().assert_required_extensions())
				output::error() << "this system does not support the required vulkan components";

			auto dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeatures {true};
			auto buffer_addressing =
				vk::PhysicalDeviceBufferDeviceAddressFeatures {true, false, false, &dynamic_rendering};

			auto descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures {};
			descriptor_indexing.shaderUniformBufferArrayNonUniformIndexing = true;
			descriptor_indexing.shaderStorageBufferArrayNonUniformIndexing = true;
			descriptor_indexing.shaderSampledImageArrayNonUniformIndexing = true;
			descriptor_indexing.descriptorBindingSampledImageUpdateAfterBind = true;
			descriptor_indexing.runtimeDescriptorArray = true;
			descriptor_indexing.descriptorBindingVariableDescriptorCount = true;
			descriptor_indexing.descriptorBindingPartiallyBound = true;
			descriptor_indexing.pNext = &buffer_addressing;

			auto descriptor_buffering =
				vk::PhysicalDeviceDescriptorBufferFeaturesEXT {true, false, false, true, &descriptor_indexing};
			auto shader_object = vk::PhysicalDeviceShaderObjectFeaturesEXT {true, &descriptor_buffering};
			auto vertex_input = vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT {true, &shader_object};
			auto synchronization2 = vk::PhysicalDeviceSynchronization2Features {true, &vertex_input};
			auto host_image_copy = vk::PhysicalDeviceHostImageCopyFeaturesEXT {true, &synchronization2};
			auto maintenantce_5 = vk::PhysicalDeviceMaintenance5FeaturesKHR {true, &host_image_copy};

			auto device_info = vk::DeviceCreateInfo {
				{}, create_info.m_queues, {}, create_info.m_extensions, &null_features, &maintenantce_5};

			m_object = {*physical_device, device_info};
		}

		auto logical_device::info() const -> lh::string::string_t
		{
			return {};
		}
	}
}
