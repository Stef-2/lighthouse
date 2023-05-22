#pragma once

#include "lighthouse/vulkan/descriptor_collection.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"
#include "lighthouse/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/vulkan/buffer.hpp"

#include "vulkan/utils/math.hpp"

#pragma optimize("", off)
lh::vulkan::descriptor_collection::descriptor_collection(const physical_device& physical_device,
														 const logical_device& logical_device,
														 const descriptor_set_layout& descriptor_set_layout,
														 const memory_allocator& memory_allocator,
														 const create_info& create_info)
	: m_data_buffers {}, m_descriptor_buffers {}
{
	const auto collection_size = descriptor_set_layout.bindings().size();

	const auto descriptor_buffer_properties = physical_device.properties().m_descriptor_buffer_properties;
	const auto descriptor_layout_size = descriptor_set_layout->getSizeEXT();
	const auto descriptor_offsets =
		std::ranges::fold_left(descriptor_set_layout.bindings(),
							   std::vector<vk::DeviceSize> {},
							   [&descriptor_set_layout](auto offsets, const auto& binding) {
								   offsets.push_back(descriptor_set_layout->getBindingOffsetEXT(binding.m_location));
								   return offsets;
							   });

	m_data_buffers.reserve(collection_size);
	m_descriptor_buffers.reserve(collection_size);

	for (std::size_t i {}; i < collection_size; i++)
	{
		const auto descriptor_size = descriptor_layout_size; // i > 0 ? descriptor_offsets[i] - descriptor_offsets[i -
															 // 1] : descriptor_offsets[1];

		const auto descriptor_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible |
												  vk::MemoryPropertyFlagBits::eHostCoherent;

		const auto data_buffer_usage = vk::BufferUsageFlagBits::eUniformBuffer |
									   vk::BufferUsageFlagBits::eShaderDeviceAddress;

		const auto descriptor_buffer_usage = vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
											 vk::BufferUsageFlagBits::eShaderDeviceAddress;

		m_data_buffers.emplace_back(physical_device,
									logical_device,
									memory_allocator,
									descriptor_size /*descriptor_set_layout.bindings()[i].m_size*/,
									mapped_buffer::create_info {.m_usage = data_buffer_usage});

		// m_data_buffers.back().map_data(vk::su::createModelViewProjectionClipMatrix({640, 320}));

		m_descriptor_buffers.push_back({physical_device,
										logical_device,
										memory_allocator,
										descriptor_size,
										mapped_buffer::create_info {.m_usage = descriptor_buffer_usage}});

		const auto desc_address = m_descriptor_buffers.back().address();

		// m_descriptor_buffers.back().map_data(vk::su::createModelViewProjectionClipMatrix({640, 320}));
		const auto address = vk::DescriptorAddressInfoEXT {m_data_buffers.back().address(),
														   m_data_buffers.back().allocation_info().size};
		const auto descriptor_data = vk::DescriptorDataEXT {&address};

		const auto descriptor_info = vk::DescriptorGetInfoEXT {vk::DescriptorType::eUniformBuffer, descriptor_data};

		const auto vk_descriptor_info = static_cast<VkDescriptorGetInfoEXT>(descriptor_info);
		auto vk_buffer = static_cast<VkBuffer>((**m_descriptor_buffers.back()));
		/*
		m_descriptor_buffers.back().allocation_info().pMappedData = logical_device->getDescriptorEXT<void*>(
			descriptor_info);*/
		/*
		const_cast<vk::Buffer&>(
			**m_descriptor_buffers.back()) = (logical_device->getDescriptorEXT<vk::Buffer>(descriptor_info));
			*/

		void* wtf = m_descriptor_buffers.back().allocation_info().pMappedData;

		logical_device->getDispatcher()->vkGetDescriptorEXT(**logical_device,
															&vk_descriptor_info,
															descriptor_buffer_properties.uniformBufferDescriptorSize,
															wtf);

		auto s = static_cast<char*>(wtf);
		std::string g {s, s + descriptor_buffer_properties.uniformBufferDescriptorSize};
		std::cout << "data ?: " << g;
	}
}

auto lh::vulkan::descriptor_collection::descriptor_buffers() -> std::vector<mapped_buffer>&
{
	return m_descriptor_buffers;
}

auto lh::vulkan::descriptor_collection::data_buffers() -> std::vector<mapped_buffer>&
{
	return m_data_buffers;
}
