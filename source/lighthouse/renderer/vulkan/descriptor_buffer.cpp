module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif

module descriptor_buffer;

import output;

namespace lh
{
	namespace vulkan
	{
		descriptor_buffer::descriptor_buffer(const physical_device& physical_device,
											 const logical_device& logical_device,
											 const memory_allocator& memory_allocator,
											 const global_descriptor& global_descriptor,
											 const create_info& create_info)
			: m_physical_device {physical_device},
			  m_logical_device {logical_device},
			  m_bind_point {create_info.m_bind_point},
			  m_resource_descriptor_buffer_binding_info {},
			  m_combined_image_sampler_descriptor_buffer_binding_info {},
			  m_resource_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  global_descriptor.uniform_buffer_set().getSizeEXT() +
					  global_descriptor.storage_descriptor_set().getSizeEXT(),
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties =
												  create_info.m_descriptor_buffer_memory_properties}},
			  m_combined_image_sampler_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  global_descriptor.combined_image_sampler_set().getSizeEXT(),
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT,
											  .m_memory_properties = create_info.m_descriptor_buffer_memory_properties}}
		{}

		auto descriptor_buffer::map_uniform_buffer_data(const binding_slot_t& offset,
														const buffer_subdata& buffer_subdata) -> void
		{
			const auto& descriptor_buffer_properties = m_physical_device.properties().m_descriptor_buffer_properties;
			const auto descriptor_offset = descriptor_buffer_properties.m_uniform_buffer_offset;
			const auto binding_slot_offset = offset * descriptor_offset;

			for (auto i = binding_slot_t {}; i < buffer_subdata.m_subdata.size(); i++)
			{
				m_resource_descriptor_buffer_binding_info.emplace_back(
					m_resource_descriptor_buffer.address() +
						i * utility::aligned_size(
								static_cast<vk::DeviceSize>(descriptor_offset),
								descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment) +
						i * binding_slot_offset,
					vk::BufferUsageFlagBits::eShaderDeviceAddress |
						vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

				const auto data_address_info = vk::DescriptorAddressInfoEXT {buffer_subdata.m_buffer->address() +
																				 buffer_subdata.m_subdata[i].m_offset,
																			 buffer_subdata.m_subdata[i].m_size};

				void* data_address = static_cast<std::byte*>(
										 m_resource_descriptor_buffer.allocation_info().pMappedData) +
									 i * descriptor_offset + binding_slot_offset;
				m_logical_device->getDescriptorEXT(
					{vk::DescriptorType::eUniformBuffer, {&data_address_info}},
					descriptor_buffer_properties.m_properties.uniformBufferDescriptorSize,
					data_address);
			}
		}

		auto descriptor_buffer::map_material(const material& material) -> void
		{
			const auto& descriptor_buffer_properties = m_physical_device.properties().m_descriptor_buffer_properties;
			const auto descriptor_offset = descriptor_buffer_properties.m_combined_image_sampler_offset;

			m_combined_image_sampler_descriptor_buffer_binding_info = {};
			m_combined_image_sampler_descriptor_buffer_binding_info.reserve(material.textures().size());

			for (auto i = std::uint32_t {}; const auto& texture : material.textures())
			{
				m_combined_image_sampler_descriptor_buffer_binding_info.emplace_back(
					m_combined_image_sampler_descriptor_buffer.address() +
						i * utility::aligned_size(
								static_cast<vk::DeviceSize>(descriptor_offset),
								descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment),
					vk::BufferUsageFlagBits::eShaderDeviceAddress |
						vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT);

				auto destination = static_cast<std::byte*>(
									   m_combined_image_sampler_descriptor_buffer.allocation_info().pMappedData) +
								   i * descriptor_offset;
				std::memcpy(destination, texture.descriptor().data(), texture.descriptor().size());

				i++;
			}
		}
#pragma optimize("", off)
		auto descriptor_buffer::map_resource_buffer(const descriptor_resource_buffer& resource_buffer) -> void
		{
			const auto& descriptor_buffer_properties = m_physical_device.properties().m_descriptor_buffer_properties;

			m_resource_descriptor_buffer_binding_info = {};
			m_resource_descriptor_buffer_binding_info.reserve(resource_buffer.descriptors().size());

			for (auto combined_offset = vk::DeviceSize {}, combined_alligned_offset = vk::DeviceSize {};
				 const auto& [descriptor_type, descriptor_data] : resource_buffer.descriptors())
			{
				const auto descriptor_offset = descriptor_data.size();

				const auto alligned_offset =
					utility::aligned_size(static_cast<vk::DeviceSize>(descriptor_offset),
										  descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

				m_resource_descriptor_buffer_binding_info.emplace_back(
					m_resource_descriptor_buffer.address() + combined_alligned_offset,
					vk::BufferUsageFlagBits::eShaderDeviceAddress |
						vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

				auto destination = static_cast<std::byte*>(m_resource_descriptor_buffer.allocation_info().pMappedData) +
								   combined_offset;

				std::memcpy(destination, descriptor_data.data(), descriptor_data.size());

				combined_offset += descriptor_offset;
				combined_alligned_offset += alligned_offset;
			}
		}

		auto descriptor_buffer::resource_buffer() -> const mapped_buffer&
		{
			return m_resource_descriptor_buffer;
		}

		auto descriptor_buffer::combined_image_sampler_buffer() -> const mapped_buffer&
		{
			return m_combined_image_sampler_descriptor_buffer;
		}

		auto descriptor_buffer::bind(const vk::raii::CommandBuffer& command_buffer,
									 const vk::raii::PipelineLayout& pipeline_layout) const -> void
		{
			auto combined_descriptor_bindings = m_resource_descriptor_buffer_binding_info;
			combined_descriptor_bindings.insert_range(combined_descriptor_bindings.end(),
													  m_combined_image_sampler_descriptor_buffer_binding_info);

			command_buffer.bindDescriptorBuffersEXT(combined_descriptor_bindings);

			std::vector<std::uint32_t> indices {
				0, 0, static_cast<std::uint32_t>(m_resource_descriptor_buffer_binding_info.size())};
			std::vector<vk::DeviceSize> offsets {0, 0, 0};

			command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 0, indices, offsets);
			// command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 0, {0}, {0});
			// command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 1, {0}, {0});
			// command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 2, {7}, {0});
		}
	}
}
