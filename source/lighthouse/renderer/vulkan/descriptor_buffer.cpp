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
			  m_bind_point {create_info.m_bind_point},
			  m_uniform_descriptor_buffer_binding_info {},
			  m_storage_descriptor_buffer_binding_info {},
			  m_combined_image_sampler_descriptor_buffer_binding_info {},
			  m_accumulated_uniform_descriptor_index {},
			  m_accumulated_storage_descriptor_index {},
			  m_vacant_combined_image_sampler_slots {},
			  m_resource_buffer_indices {},
			  m_uniform_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  /*global_descriptor.uniform_buffer_set().getSizeEXT()*/ 256 * 256,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties =
												  create_info.m_descriptor_buffer_memory_properties}},
			  m_storage_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  /*global_descriptor.storage_descriptor_set().getSizeEXT()*/ 256 * 256,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties =
												  create_info.m_descriptor_buffer_memory_properties}},
			  m_combined_image_sampler_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  /*global_descriptor.combined_image_sampler_set().getSizeEXT()*/ 256 * 256,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
														 vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT,
											  .m_memory_properties = create_info.m_descriptor_buffer_memory_properties}}
		{
			m_uniform_descriptor_buffer_binding_info.reserve(
				global_descriptor.create_information().m_num_uniform_buffers);
			m_storage_descriptor_buffer_binding_info.reserve(
				global_descriptor.create_information().m_num_storage_descriptors);
			m_combined_image_sampler_descriptor_buffer_binding_info.reserve(
				global_descriptor.create_information().m_num_combined_image_samplers);
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
			const auto aligned_uniform_descriptor_offset = utility::aligned_size(
				uniform_descriptor_offset, descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

			const auto storage_descriptor_size = descriptor_buffer_properties.m_properties.storageBufferDescriptorSize;
			const auto& storage_descriptor_offset = descriptor_buffer_properties.m_storage_buffer_offset;
			const auto aligned_storage_descriptor_offset = utility::aligned_size(
				storage_descriptor_offset, descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

			// associate this resource buffer with indices into the descriptor buffer
			const auto accumulated_uniform_descriptor_index = m_accumulated_uniform_descriptor_index;
			const auto aligned_uniform_descriptor_size = utility::aligned_size(
				uniform_descriptor_size, descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);
			const auto accumulated_storage_descriptor_index = m_accumulated_storage_descriptor_index;
			m_resource_buffer_indices.insert_or_assign(&resource_buffer,
													   resource_buffer_offsets {accumulated_uniform_descriptor_index,
																				accumulated_storage_descriptor_index});
			// register uniform buffers
			for (auto i = global_descriptor::descriptor_type_size_t {};
				 const auto& descriptor_data : resource_buffer.uniform_descriptors())
			{
				const auto memcpy_destination = static_cast<std::byte*>(
													m_uniform_descriptor_buffer.mapped_data_pointer()) +
												accumulated_uniform_descriptor_index * descriptor_size +
												i * descriptor_size;

				m_uniform_descriptor_buffer_binding_info.emplace_back(
					m_uniform_descriptor_buffer.address() + aligned_uniform_descriptor_offset * i,
					vk::BufferUsageFlagBits::eShaderDeviceAddress |
						vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

				std::memcpy(memcpy_destination, descriptor_data.data(), descriptor_size);
				m_accumulated_uniform_descriptor_index++;
				i++;
			}

			// register storage buffers
			for (auto i = global_descriptor::descriptor_type_size_t {};
				 const auto& descriptor_data : resource_buffer.storage_descriptors())
			{
				m_storage_descriptor_buffer_binding_info.emplace_back(
					m_storage_descriptor_buffer.address() + aligned_storage_descriptor_offset * i,
					vk::BufferUsageFlagBits::eShaderDeviceAddress |
						vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

				const auto memcpy_destination = static_cast<std::byte*>(
													m_storage_descriptor_buffer.mapped_data_pointer()) +
												accumulated_storage_descriptor_index * descriptor_size +
												i * descriptor_size;

				std::memcpy(memcpy_destination, descriptor_data.data(), descriptor_size);
				m_accumulated_storage_descriptor_index++;
				i++;
			}
		}

		auto descriptor_buffer::map_resource_buffer_offsets(const vk::raii::CommandBuffer& command_buffer,
															const descriptor_resource_buffer& resource_buffer) const
			-> void
		{
			const auto& resource_indices = m_resource_buffer_indices.at(&resource_buffer);

			constexpr auto uniform_descriptor_index = std::uint32_t {};
			const auto storage_descriptor_index = static_cast<std::uint32_t>(
				m_uniform_descriptor_buffer_binding_info.size());
			const auto combined_image_sampler_descriptor_index = static_cast<std::uint32_t>(
				m_uniform_descriptor_buffer_binding_info.size() + m_storage_descriptor_buffer_binding_info.size());

			std::array<std::uint32_t, 3> indices {uniform_descriptor_index +
													  resource_indices.m_uniform_descriptor_offset,
												  storage_descriptor_index +
													  resource_indices.m_storage_descriptor_offset,
												  combined_image_sampler_descriptor_index};

			// std::cout << "db indices: " << indices[0] << " " << indices[1] << " " << indices[2] << " " << '\n';
			/*
			if (resource_indices.m_uniform_descriptor_offset == 2)
			{
				command_buffer.setDescriptorBufferOffsetsEXT(
					m_bind_point, *m_global_descriptor.pipeline_layout(), 0, {0, 1, 2}, {0, 0, 0});
				return;
			}*/

			command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point,
														 *m_global_descriptor.pipeline_layout(),
														 0,
														 {0, 1, 2},
														 {resource_indices.m_uniform_descriptor_offset * 256,
														  resource_indices.m_storage_descriptor_offset * 256,
														  0});
			/*
			command_buffer.setDescriptorBufferOffsetsEXT(
				m_bind_point, *m_global_descriptor.pipeline_layout(), 0, {0}, {0});

			command_buffer.setDescriptorBufferOffsetsEXT(
				m_bind_point, *m_global_descriptor.pipeline_layout(), 1, {1}, {0});

			command_buffer.setDescriptorBufferOffsetsEXT(
				m_bind_point, *m_global_descriptor.pipeline_layout(), 2, {2}, {0});*/
		}

		auto descriptor_buffer::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			auto combined_descriptor_bindings = m_uniform_descriptor_buffer_binding_info;
			combined_descriptor_bindings.insert_range(combined_descriptor_bindings.end(),
													  m_storage_descriptor_buffer_binding_info);
			combined_descriptor_bindings.insert_range(combined_descriptor_bindings.end(),
													  m_combined_image_sampler_descriptor_buffer_binding_info);

			const auto test = std::array<vk::DescriptorBufferBindingInfoEXT, 3> {
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
			command_buffer.bindDescriptorBuffersEXT(test);
		}

		auto descriptor_buffer::flush_resource_descriptors() -> void
		{
			m_uniform_descriptor_buffer_binding_info.clear();
			m_storage_descriptor_buffer_binding_info.clear();
			m_accumulated_uniform_descriptor_index = {};
			m_accumulated_storage_descriptor_index = {};

			m_resource_buffer_indices.clear();
		}
	}
}
