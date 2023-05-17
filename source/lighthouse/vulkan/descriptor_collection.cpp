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
	const auto collection_size = descriptor_set_layout.bindings().size();

	const auto descriptor_buffer_properties = vk::PhysicalDeviceDescriptorBufferPropertiesEXT {};
	const auto descriptor_layout_size = descriptor_set_layout->getSizeEXT();
	const auto descriptor_offsets =
		std::ranges::fold_left(descriptor_set_layout.bindings(),
							   std::vector<vk::DeviceSize> {},
							   [&descriptor_set_layout](auto offsets, const auto& binding) {
								   offsets.push_back(descriptor_set_layout->getBindingOffsetEXT(binding.m_location));
								   return offsets;
							   });

	m_descriptor_buffers.reserve(collection_size);

	for (std::size_t i {}; i < collection_size; i++)
	{
		const auto size = descriptor_layout_size; // i > 0 ? descriptor_offsets[i] - descriptor_offsets[i - 1] :
												  // descriptor_offsets[1];

		const auto usage = vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
						   vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress;

		m_descriptor_buffers.push_back(
			{{physical_device, logical_device, memory_allocator, size, buffer::create_info {.m_usage = usage}},
			 nullptr});

		m_descriptor_buffers.back().first.memory().mapMemory(0, VK_WHOLE_SIZE);

		const auto address = vk::DescriptorAddressInfoEXT {m_descriptor_buffers.back().first.address(), size};
		const auto data = vk::DescriptorDataEXT {
			&address,
		};
		const auto descriptor_info = vk::DescriptorGetInfoEXT {vk::DescriptorType::eUniformBuffer, data};

		m_descriptor_buffers.back().second = logical_device->getDescriptorEXT<void*>(descriptor_info);
	}
}

auto lh::vulkan::descriptor_collection::descriptor_buffers() const -> const std::vector<std::pair<buffer, void*>>&
{
	return m_descriptor_buffers;
}