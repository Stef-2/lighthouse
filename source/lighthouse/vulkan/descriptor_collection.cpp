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
														 const buffer_subdata& data,
														 const create_info& create_info)
	: m_descriptor_buffer {}, m_binding_info {}, m_bind_point {create_info.m_bind_point}
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

	const auto descriptor_memory_properties = vk::MemoryPropertyFlagBits::eHostVisible |
											  vk::MemoryPropertyFlagBits::eHostCoherent;

	const auto descriptor_buffer_usage = vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
										 vk::BufferUsageFlagBits::eShaderDeviceAddress;

	m_descriptor_buffer = std::make_unique<mapped_buffer>(
		physical_device,
		logical_device,
		memory_allocator,
		collection_size * descriptor_set_layout.bindings().size(),
		mapped_buffer::create_info {.m_usage = descriptor_buffer_usage, .m_properties = descriptor_memory_properties});

	m_binding_info1 = {m_descriptor_buffer->address(), vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT};
	m_binding_info2 = {m_descriptor_buffer->address() + 256, vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT};

	const auto data_address_info1 = vk::DescriptorAddressInfoEXT {data->address(), 64};
	const auto data_address_info2 = vk::DescriptorAddressInfoEXT {data->address() + 256, 4};

	const auto descriptor_data1 = vk::DescriptorDataEXT {&data_address_info1};
	const auto descriptor_data2 = vk::DescriptorDataEXT {&data_address_info2};

	const auto& descriptor_info1 = static_cast<VkDescriptorGetInfoEXT>(
		vk::DescriptorGetInfoEXT {descriptor_set_layout.bindings().front().m_type, descriptor_data1});
	const auto& descriptor_info2 = static_cast<VkDescriptorGetInfoEXT>(
		vk::DescriptorGetInfoEXT {descriptor_set_layout.bindings().back().m_type, descriptor_data2});

	uint8_t* p = (uint8_t*)m_descriptor_buffer->allocation_info().pMappedData;

	logical_device->getDispatcher()->vkGetDescriptorEXT(
		**logical_device,
		&descriptor_info1,
		descriptor_collection::descriptor_size(physical_device, descriptor_set_layout.bindings().front().m_type),
		p);

	logical_device->getDispatcher()->vkGetDescriptorEXT(
		**logical_device,
		&descriptor_info2,
		descriptor_collection::descriptor_size(physical_device, descriptor_set_layout.bindings().back().m_type),
		p + 8);
}

auto lh::vulkan::descriptor_collection::descriptor_buffer() -> const mapped_buffer&
{
	return *m_descriptor_buffer;
}

auto lh::vulkan::descriptor_collection::bind(const vk::raii::CommandBuffer& command_buffer,
											 const vk::raii::PipelineLayout& pipeline_layout) const -> void
{
	command_buffer.bindDescriptorBuffersEXT({m_binding_info1, m_binding_info2});

	command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 0, {0}, {0});
	/*
	uint32_t indices[] = {0, 1};
	VkDeviceSize offsets[] = {0, 256};
	command_buffer.getDispatcher()->vkCmdSetDescriptorBufferOffsetsEXT(
		*command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline_layout, 0, 1, indices, offsets);*/

	// command_buffer.bindDescriptorBuffersEXT({m_binding_info2});
	/*
	command_buffer.getDispatcher()->vkCmdSetDescriptorBufferOffsetsEXT(
		*command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline_layout, 0, 1, &indices[0], &offsets[1]);*/
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

auto lh::vulkan::descriptor_collection::descriptor_usage(const descriptor_set_layout& descriptor_set_layout)
	-> const vk::BufferUsageFlags
{
	auto usage = vk::BufferUsageFlags {};

	for (const auto& binding : descriptor_set_layout.bindings())
	{
		if (binding.m_type == vk::DescriptorType::eStorageBuffer or
			binding.m_type == vk::DescriptorType::eStorageBufferDynamic or
			binding.m_type == vk::DescriptorType::eUniformBuffer or
			binding.m_type == vk::DescriptorType::eUniformBufferDynamic)

			usage &= vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT;

		if (binding.m_type == vk::DescriptorType::eCombinedImageSampler or
			binding.m_type == vk::DescriptorType::eSampler)

			usage &= vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT;
	}

	return usage;
}
