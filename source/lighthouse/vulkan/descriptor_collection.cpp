#pragma once

#include "lighthouse/vulkan/descriptor_collection.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"
#include "lighthouse/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/vulkan/buffer.hpp"
#include "lighthouse/vulkan/utility.hpp"

#pragma optimize("", off)
lh::vulkan::descriptor_collection::descriptor_collection(const physical_device& physical_device,
														 const logical_device& logical_device,
														 const memory_allocator& memory_allocator,
														 const descriptor_set_layout& descriptor_set_layout,
														 const std::vector<non_owning_ptr<mapped_buffer>>& data,
														 const create_info& create_info)
	: m_descriptor_buffers {}, m_binding_info {}
{
	const auto collection_size = descriptor_set_layout.bindings().size();

	const auto descriptor_buffer_properties = physical_device.properties().m_descriptor_buffer_properties;
	const auto descriptor_layout_size = descriptor_set_layout->getSizeEXT();
	const auto descriptor_offsets =
		std::ranges::fold_left(descriptor_set_layout.bindings(),
							   std::vector<vk::DeviceSize> {},
							   [&descriptor_set_layout](auto offsets, const auto& binding) {
								   offsets.push_back(descriptor_set_layout->getBindingOffsetEXT(binding.m_binding));
								   return std::move(offsets);
							   });

	m_descriptor_buffers.reserve(collection_size);
	m_binding_info.reserve(collection_size);

	for (std::size_t i {}; i < collection_size; i++)
	{
		const auto descriptor_size = descriptor_layout_size;

		const auto descriptor_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible |
												  vk::MemoryPropertyFlagBits::eHostCoherent;

		const auto descriptor_buffer_usage = vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
											 vk::BufferUsageFlagBits::eShaderDeviceAddress;

		m_descriptor_buffers.emplace_back(physical_device,
										  logical_device,
										  memory_allocator,
										  descriptor_size,
										  mapped_buffer::create_info {.m_usage = descriptor_buffer_usage});

		m_binding_info.emplace_back(m_descriptor_buffers.back().address(),
									vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

		const auto data_address_info = vk::DescriptorAddressInfoEXT {data[i]->address(), data[i]->size()};

		const auto descriptor_data = vk::DescriptorDataEXT {&data_address_info};

		const auto& descriptor_info = static_cast<VkDescriptorGetInfoEXT>(
			vk::DescriptorGetInfoEXT {descriptor_set_layout.bindings()[i].m_type, descriptor_data});

		logical_device->getDispatcher()->vkGetDescriptorEXT(
			**logical_device,
			&descriptor_info,
			descriptor_collection::descriptor_size(physical_device, descriptor_set_layout.bindings()[i].m_type),
			m_descriptor_buffers[i].allocation_info().pMappedData);
	}
}

auto lh::vulkan::descriptor_collection::descriptor_buffers() -> std::vector<mapped_buffer>&
{
	return m_descriptor_buffers;
}

auto lh::vulkan::descriptor_collection::bind(const vk::raii::CommandBuffer& command_buffer,
											 const vk::raii::PipelineLayout& pipeline_layout) const -> void
{
	command_buffer.bindDescriptorBuffersEXT(m_binding_info);
	command_buffer.setDescriptorBufferOffsetsEXT(vk::PipelineBindPoint::eGraphics, *pipeline_layout, 0, {0}, {0});
}

auto lh::vulkan::descriptor_collection::descriptor_size(const physical_device& physical_device,
														const vk::DescriptorType& descriptor_type) -> const std::size_t
{
	const auto& descriptor_properties = physical_device.properties().m_descriptor_buffer_properties;

	switch (descriptor_type)
	{
		case vk::DescriptorType::eUniformBuffer: return descriptor_properties.uniformBufferDescriptorSize;
		case vk::DescriptorType::eCombinedImageSampler: return descriptor_properties.combinedImageSamplerDescriptorSize;
		case vk::DescriptorType::eStorageBuffer: return descriptor_properties.storageBufferDescriptorSize;
		default: break;
	}

	std::unreachable();
}
