module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif

module descriptor_buffer;

import vulkan_utility;
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
			  m_global_descriptor {global_descriptor},
			  m_accumulated_uniform_descriptor_offset {},
			  m_accumulated_storage_descriptor_offset {},
			  m_resource_buffer_offsets {},
			  m_texture_count {},
			  m_vacant_combined_image_sampler_slots {},
			  m_uniform_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  global_descriptor.uniform_buffer_set().getSizeEXT() *
					  global_descriptor.create_information().m_num_uniform_buffers,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties =
												  create_info.m_descriptor_buffer_memory_properties}},
			  m_storage_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  global_descriptor.storage_descriptor_set().getSizeEXT() *
					  global_descriptor.create_information().m_num_storage_descriptors,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties =
												  create_info.m_descriptor_buffer_memory_properties}},
			  m_combined_image_sampler_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  global_descriptor.combined_image_sampler_set().getSizeEXT() *
					  global_descriptor.create_information().m_num_combined_image_samplers,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
														 vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT,
											  .m_memory_properties = create_info.m_descriptor_buffer_memory_properties}}
		{
			/*
			std::cout << "uniform desc buff sz: " << global_descriptor.uniform_buffer_set().getSizeEXT() << '\n';
			for (auto i = 0; i < global_descriptor.create_information().m_num_uniform_buffers; i++)
			{
				std::cout << "offset @: " << i << " = " << global_descriptor.uniform_buffer_set().getBindingOffsetEXT(i)
						  << '\n';
			}
			std::cout << "storage desc buff sz: " << global_descriptor.storage_descriptor_set().getSizeEXT() << '\n';
			for (auto i = 0; i < global_descriptor.create_information().m_num_storage_descriptors; i++)
			{
				std::cout << "offset @: " << i << " = "
						  << global_descriptor.storage_descriptor_set().getBindingOffsetEXT(i) << '\n';
			}
			std::cout << "cis desc buff sz: " << global_descriptor.combined_image_sampler_set().getSizeEXT() << '\n';
			for (auto i = 0; i < global_descriptor.create_information().m_num_combined_image_samplers; i++)
			{
				std::cout << "offset @: " << i << " = "
						  << global_descriptor.combined_image_sampler_set().getBindingOffsetEXT(i) << '\n';
			}*/
		}

		auto descriptor_buffer::register_resource_buffer(const descriptor_resource_buffer& resource_buffer) const
			-> void
		{
			// descriptor properties assigned by the implementation
			const auto& descriptor_buffer_properties = m_physical_device.properties().m_descriptor_buffer_properties;

			const auto uniform_descriptor_size = descriptor_buffer_properties.m_properties.uniformBufferDescriptorSize;
			const auto aligned_uniform_descriptor_size = utility::aligned_size(
				uniform_descriptor_size, descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);
			const auto& uniform_descriptor_offset = descriptor_buffer_properties.m_uniform_buffer_offset;
			const auto aligned_uniform_descriptor_offset =
				utility::aligned_size(static_cast<vk::DeviceSize>(uniform_descriptor_offset),
									  descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

			const auto storage_descriptor_size = descriptor_buffer_properties.m_properties.storageBufferDescriptorSize;
			const auto aligned_storage_descriptor_size = utility::aligned_size(
				storage_descriptor_size, descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);
			const auto& storage_descriptor_offset = descriptor_buffer_properties.m_storage_buffer_offset;
			const auto aligned_storage_descriptor_offset =
				utility::aligned_size(static_cast<vk::DeviceSize>(storage_descriptor_offset),
									  descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

			// associate this resource buffer with indices into the descriptor buffer
			const auto accumulated_uniform_descriptor_offset = m_accumulated_uniform_descriptor_offset;
			const auto accumulated_storage_descriptor_offset = m_accumulated_storage_descriptor_offset;
			m_resource_buffer_offsets.insert_or_assign(&resource_buffer,
													   resource_buffer_offsets {accumulated_uniform_descriptor_offset,
																				accumulated_storage_descriptor_offset});
			// register uniform buffers
			for (auto i = global_descriptor::descriptor_type_size_t {};
				 const auto& descriptor_data : resource_buffer.uniform_descriptors())
			{
				const auto memcpy_destination = static_cast<std::byte*>(
													m_uniform_descriptor_buffer.mapped_data_pointer()) +
												accumulated_uniform_descriptor_offset + i * uniform_descriptor_size;

				std::memcpy(memcpy_destination, descriptor_data.data(), uniform_descriptor_size);
				m_accumulated_uniform_descriptor_offset += aligned_uniform_descriptor_offset;
				i++;
			}

			// register storage buffers
			for (auto i = global_descriptor::descriptor_type_size_t {};
				 const auto& descriptor_data : resource_buffer.storage_descriptors())
			{
				const auto memcpy_destination = static_cast<std::byte*>(
													m_storage_descriptor_buffer.mapped_data_pointer()) +
												accumulated_storage_descriptor_offset + i * storage_descriptor_size;

				std::memcpy(memcpy_destination, descriptor_data.data(), storage_descriptor_size);
				m_accumulated_storage_descriptor_offset += aligned_storage_descriptor_offset;
				i++;
			}
		}

		auto descriptor_buffer::map_resource_buffer_offsets(const vk::raii::CommandBuffer& command_buffer,
															const descriptor_resource_buffer& resource_buffer,
															const vk::PipelineBindPoint& bind_point) const -> void
		{
			const auto& resource_offsets = m_resource_buffer_offsets.at(&resource_buffer);

			command_buffer.setDescriptorBufferOffsetsEXT(bind_point,
														 *m_global_descriptor.pipeline_layout(),
														 0,
														 {0, 1, 2},
														 {resource_offsets.m_uniform_descriptor_offset,
														  resource_offsets.m_storage_descriptor_offset,
														  0});
		}

		auto descriptor_buffer::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			const auto bindings = std::array<vk::DescriptorBufferBindingInfoEXT, 3> {
				vk::DescriptorBufferBindingInfoEXT {m_uniform_descriptor_buffer.address(),
													vk::BufferUsageFlagBits::eShaderDeviceAddress |
														vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT},
				vk::DescriptorBufferBindingInfoEXT {m_storage_descriptor_buffer.address(),
													vk::BufferUsageFlagBits::eShaderDeviceAddress |
														vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT},
				vk::DescriptorBufferBindingInfoEXT {m_combined_image_sampler_descriptor_buffer.address(),
													vk::BufferUsageFlagBits::eShaderDeviceAddress |
														vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
														vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT}};
			command_buffer.bindDescriptorBuffersEXT(bindings);
		}

		auto descriptor_buffer::flush_resource_descriptors() -> void
		{
			m_accumulated_uniform_descriptor_offset = {};
			m_accumulated_storage_descriptor_offset = {};

			m_resource_buffer_offsets.clear();
		}
	}
}
