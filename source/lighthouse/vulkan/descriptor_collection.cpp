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
	const auto assert_binding_and_data_matching = descriptor_set_layout.bindings().size() == data.m_subdata.size();
	if (not assert_binding_and_data_matching)
		output::error() << "descriptor set layout bindings and provided buffer data do not match";

	const auto descriptor_buffer_properties = physical_device.properties().m_descriptor_buffer_properties;
	m_binding_info.reserve(descriptor_set_layout.bindings().size());

	m_descriptor_buffer = std::make_unique<mapped_buffer>(
		physical_device,
		logical_device,
		memory_allocator,
		descriptor_set_layout->getSizeEXT() * descriptor_set_layout.bindings().size(),
		mapped_buffer::create_info {.m_usage = descriptor_buffer_usage(descriptor_set_layout),
									.m_properties = create_info.descriptor_collection_memory_properties});

	for (std::size_t i {}; const auto& binding : descriptor_set_layout.bindings())
	{
		const auto binding_offset = descriptor_set_layout->getBindingOffsetEXT(binding.m_binding);

		m_binding_info.emplace_back(
			m_descriptor_buffer->address() +
				i * utility::aligned_size(binding_offset, descriptor_buffer_properties.descriptorBufferOffsetAlignment),
			descriptor_buffer_usage(descriptor_set_layout));

		const auto data_address_info = vk::DescriptorAddressInfoEXT {data.m_buffer->address() +
																		 data.m_subdata[i].m_offset,
																	 data.m_subdata[i].m_size};

		const auto& descriptor_info = static_cast<VkDescriptorGetInfoEXT>(
			vk::DescriptorGetInfoEXT {binding.m_type, {&data_address_info}});

		logical_device->getDispatcher()->vkGetDescriptorEXT(
			**logical_device,
			&descriptor_info,
			descriptor_collection::descriptor_size(physical_device, binding.m_type),
			static_cast<std::byte*>(m_descriptor_buffer->allocation_info().pMappedData) + binding_offset);
		i++;
	}
}

auto lh::vulkan::descriptor_collection::descriptor_buffer() -> const mapped_buffer&
{
	return *m_descriptor_buffer;
}

auto lh::vulkan::descriptor_collection::bind(const vk::raii::CommandBuffer& command_buffer,
											 const vk::raii::PipelineLayout& pipeline_layout) const -> void
{
	command_buffer.bindDescriptorBuffersEXT(m_binding_info);
	command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 0, {0}, {0});
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

auto lh::vulkan::descriptor_collection::descriptor_buffer_usage(const descriptor_set_layout& descriptor_set_layout)
	-> const vk::BufferUsageFlags
{
	auto usage = vk::BufferUsageFlags {vk::BufferUsageFlagBits::eShaderDeviceAddress};

	for (const auto& binding : descriptor_set_layout.bindings())
	{
		if (binding.m_type == vk::DescriptorType::eStorageBuffer or
			binding.m_type == vk::DescriptorType::eStorageBufferDynamic or
			binding.m_type == vk::DescriptorType::eUniformBuffer or
			binding.m_type == vk::DescriptorType::eUniformBufferDynamic)

			usage = usage | vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT;

		if (binding.m_type == vk::DescriptorType::eCombinedImageSampler or
			binding.m_type == vk::DescriptorType::eSampler)

			usage = usage | vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT;
	}

	return usage;
}
