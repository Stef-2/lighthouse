#pragma once

#include "lighthouse/vulkan/descriptor_collection.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"
#include "lighthouse/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/vulkan/buffer.hpp"

lh::vulkan::descriptor_collection::descriptor_collection(const physical_device& physical_device,
														 const logical_device& logical_device,
														 const descriptor_set_layout& descriptor_set_layout,
														 const memory_allocator& memory_allocator,
														 const create_info& create_info)
	: m_descriptor_buffers {}
{
	const auto s = vk::PhysicalDeviceDescriptorBufferPropertiesEXT {};
	const auto descriptor_size = descriptor_set_layout->getSizeEXT();
	const auto descriptor_offsets =
		std::ranges::fold_left(descriptor_set_layout.bindings(),
							   std::vector<vk::DeviceSize> {},
							   [&descriptor_set_layout](auto offsets, const auto& binding) {
								   offsets.push_back(descriptor_set_layout->getBindingOffsetEXT(binding.m_location));
								   return offsets;
							   });

	for (std::size_t i {}; i < descriptor_set_layout.bindings().size(); i++)
	{
		const auto size = i > 0 ? descriptor_offsets[i] - descriptor_offsets[i - 1] : 0;

		const auto usage = vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
						   vk::BufferUsageFlagBits::eUniformBuffer;

		m_descriptor_buffers.emplace_back(
			physical_device, logical_device, memory_allocator, size, buffer::create_info {.m_usage = usage});

		m_descriptor_buffers.back().memory().mapMemory(0, VK_WHOLE_SIZE);

		const auto address = vk::DescriptorAddressInfoEXT {m_descriptor_buffers.back().address(), size};
		const auto data = vk::DescriptorDataEXT {&address};
		const auto descriptor_info = vk::DescriptorGetInfoEXT {vk::DescriptorType::eUniformBuffer, data};

		void* p = logical_device->getDescriptorEXT<void*>(descriptor_info);

		auto b = logical_device->getDescriptorEXT<vk::DescriptorType>(descriptor_info);
		// vkGetDescriptorEXT(**logical_device,&((VkDescriptorGetInfoEXT)descriptor_info),size,)
	}

	//	logical_device->getDescriptorEXT()
}
