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
			  m_accumulated_uniform_descriptor_offset {},
			  m_accumulated_storage_descriptor_offset {},
			  m_vacant_combined_image_sampler_slots {},
			  m_resource_buffer_offsets {},
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

			m_resource_buffer_offsets.emplace_back(resource_buffer,
												   m_accumulated_uniform_descriptor_offset,
												   m_accumulated_storage_descriptor_offset);

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
				m_accumulated_uniform_descriptor_offset += alligned_offset;
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
				m_accumulated_storage_descriptor_offset += alligned_offset;
				i++;
			}
		}

		auto descriptor_buffer::map_resource_buffer_offsets(const vk::raii::CommandBuffer& command_buffer,
															const descriptor_resource_buffer& resource_buffer) -> void
		{
			const auto wtf = std::ranges::find_if(m_resource_buffer_offsets, [&resource_buffer](const auto& element) {
				return element.m_descriptor_resource_buffer == resource_buffer;
			});

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

			command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point,
														 *m_global_descriptor.pipeline_layout(),
														 0,
														 indices,
														 {wtf->m_uniform_descriptor_offset,
														  wtf->m_storage_descriptor_offset,
														  0});
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
			m_accumulated_uniform_descriptor_offset = {};
			m_accumulated_storage_descriptor_offset = {};

			m_resource_buffer_offsets.clear();
		}
	}
}
