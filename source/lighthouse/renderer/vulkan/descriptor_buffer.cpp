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
														 vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT,
											  .m_memory_properties = create_info.m_descriptor_buffer_memory_properties}}
		{
			m_uniform_descriptor_buffer_binding_info.reserve(
				global_descriptor.create_information().m_num_uniform_buffers);
			m_storage_descriptor_buffer_binding_info.reserve(
				global_descriptor.create_information().m_num_storage_descriptors);
			m_combined_image_sampler_descriptor_buffer_binding_info.reserve(
				global_descriptor.create_information().m_num_combined_image_samplers);
		}

		auto descriptor_buffer::register_resource_buffer(const descriptor_resource_buffer& resource_buffer) -> void
		{
			const auto& descriptor_buffer_properties = m_physical_device.properties().m_descriptor_buffer_properties;

			m_resource_buffer_indices.insert_or_assign(
				&resource_buffer,
				resource_buffer_offsets {m_accumulated_uniform_descriptor_index,
										 m_accumulated_storage_descriptor_index});

			// register uniform buffers
			for (auto i = global_descriptor::descriptor_type_size_t {};
				 const auto& descriptor_data : resource_buffer.uniform_descriptors())
			{
				const auto descriptor_size = descriptor_buffer_properties.m_properties.uniformBufferDescriptorSize;

				const auto alligned_offset =
					utility::aligned_size(static_cast<vk::DeviceSize>(descriptor_size),
										  descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

				const auto memcpy_destination = static_cast<std::byte*>(
													m_uniform_descriptor_buffer.mapped_data_pointer()) +
												i * descriptor_size;

				m_uniform_descriptor_buffer_binding_info.emplace_back(
					m_uniform_descriptor_buffer.address() + alligned_offset * i,
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
				const auto descriptor_size = descriptor_buffer_properties.m_properties.storageBufferDescriptorSize;

				const auto alligned_offset =
					utility::aligned_size(static_cast<vk::DeviceSize>(descriptor_size),
										  descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

				m_storage_descriptor_buffer_binding_info.emplace_back(
					m_storage_descriptor_buffer.address() + alligned_offset * i,
					vk::BufferUsageFlagBits::eShaderDeviceAddress |
						vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

				const auto memcpy_destination = static_cast<std::byte*>(
													m_storage_descriptor_buffer.mapped_data_pointer()) +
												i * descriptor_size;

				std::memcpy(memcpy_destination, descriptor_data.data(), descriptor_size);
				m_accumulated_storage_descriptor_index++;
				i++;
			}
		}

		auto descriptor_buffer::map_resource_buffer_offsets(const vk::raii::CommandBuffer& command_buffer,
															const descriptor_resource_buffer& resource_buffer) -> void
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
			/*
			if (resource_indices.m_uniform_descriptor_offset == 512)
			{
				auto new_indices = indices;
				new_indices[0] += 2;
				command_buffer.setDescriptorBufferOffsetsEXT(
					m_bind_point, *m_global_descriptor.pipeline_layout(), 0, new_indices, {0, 0, 0});
				return;
			}*/
			/*
			if (resource_indices.m_uniform_descriptor_offset == 2)
			{
				auto new_indices = indices;
				new_indices[0] += 2;
				command_buffer.setDescriptorBufferOffsetsEXT(
					m_bind_point, *m_global_descriptor.pipeline_layout(), 0, new_indices, {0, 0, 0});
				return;
			}*/
			command_buffer.setDescriptorBufferOffsetsEXT(
				m_bind_point, *m_global_descriptor.pipeline_layout(), 0, indices, {0, 0, 0});
		}

		auto descriptor_buffer::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			auto combined_descriptor_bindings = m_uniform_descriptor_buffer_binding_info;
			combined_descriptor_bindings.insert_range(combined_descriptor_bindings.end(),
													  m_storage_descriptor_buffer_binding_info);
			combined_descriptor_bindings.insert_range(combined_descriptor_bindings.end(),
													  m_combined_image_sampler_descriptor_buffer_binding_info);

			command_buffer.bindDescriptorBuffersEXT(combined_descriptor_bindings);
			/*
			constexpr auto uniform_descriptor_index = std::uint32_t {};
			const auto storage_descriptor_index = static_cast<std::uint32_t>(
				m_uniform_descriptor_buffer_binding_info.size());
			const auto combined_image_sampler_descriptor_index = static_cast<std::uint32_t>(
				m_uniform_descriptor_buffer_binding_info.size() + m_storage_descriptor_buffer_binding_info.size());
			const auto light_storage_descriptor_index = static_cast<std::uint32_t>(
				m_uniform_descriptor_buffer_binding_info.size() + m_storage_descriptor_buffer_binding_info.size() +
				m_combined_image_sampler_descriptor_buffer_binding_info.size());

			std::vector<std::uint32_t> indices {uniform_descriptor_index,
												storage_descriptor_index,
												combined_image_sampler_descriptor_index};

			command_buffer.setDescriptorBufferOffsetsEXT(
				m_bind_point, *m_global_descriptor.pipeline_layout(), 0, indices, {0, 0, 0});*/
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
