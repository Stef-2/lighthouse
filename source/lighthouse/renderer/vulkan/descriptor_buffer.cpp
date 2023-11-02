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
											 const global_light_descriptor_buffer& global_light_descriptor_buffer,
											 const create_info& create_info)
			: m_physical_device {physical_device},
			  m_logical_device {logical_device},
			  m_global_light_descriptor_buffer {global_light_descriptor_buffer},
			  m_bind_point {create_info.m_bind_point},
			  m_uniform_descriptor_buffer_binding_info {},
			  m_storage_descriptor_buffer_binding_info {},
			  m_combined_image_sampler_descriptor_buffer_binding_info {},
			  m_uniform_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  global_descriptor.uniform_buffer_set().getSizeEXT(),
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties =
												  create_info.m_descriptor_buffer_memory_properties}},
			  m_storage_descriptor_buffer {
				  logical_device,
				  memory_allocator,
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
											  .m_memory_properties =
												  create_info.m_descriptor_buffer_memory_properties}},
			  m_light_storage_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  global_descriptor.light_storage_set().getSizeEXT(),
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties = create_info.m_descriptor_buffer_memory_properties}}
		{
			for (auto i = light::light_stack_size_t {};
				 const auto& [descriptor_type, light_descriptor] :
				 m_global_light_descriptor_buffer.light_resource_buffer().descriptors())
			{
				auto memcpy_destination = static_cast<std::byte*>(
											  m_light_storage_descriptor_buffer.allocation_info().pMappedData) +
										  i * light_descriptor.size();

				std::memcpy(memcpy_destination, light_descriptor.data(), light_descriptor.size());

				i++;
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

				auto memcpy_destination =
					static_cast<std::byte*>(m_combined_image_sampler_descriptor_buffer.allocation_info().pMappedData) +
					i * descriptor_offset;
				std::memcpy(memcpy_destination, texture.descriptor().data(), texture.descriptor().size());

				i++;
			}
		}

		auto descriptor_buffer::map_resource_buffer(const descriptor_resource_buffer& resource_buffer) -> void
		{
			const auto& descriptor_buffer_properties = m_physical_device.properties().m_descriptor_buffer_properties;

			m_uniform_descriptor_buffer_binding_info = {};
			m_storage_descriptor_buffer_binding_info = {};

			for (auto num_uniform_descriptors = std::uint16_t {}, num_storage_descriptors = std::uint16_t {};
				 const auto& [descriptor_type, descriptor_data] : resource_buffer.descriptors())
			{
				const auto descriptor_size = descriptor_data.size();

				const auto alligned_offset =
					utility::aligned_size(static_cast<vk::DeviceSize>(descriptor_size),
										  descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

				auto memcpy_destination = static_cast<void*>(nullptr);

				if (descriptor_type == vk::DescriptorType::eUniformBuffer)
				{
					memcpy_destination = static_cast<std::byte*>(
											 m_uniform_descriptor_buffer.allocation_info().pMappedData) +
										 num_uniform_descriptors * descriptor_size;

					m_uniform_descriptor_buffer_binding_info.emplace_back(
						m_uniform_descriptor_buffer.address() + alligned_offset * num_uniform_descriptors,
						vk::BufferUsageFlagBits::eShaderDeviceAddress |
							vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

					num_uniform_descriptors++;
				} else
				{
					memcpy_destination = static_cast<std::byte*>(
											 m_storage_descriptor_buffer.allocation_info().pMappedData) +
										 num_storage_descriptors * descriptor_size;

					m_storage_descriptor_buffer_binding_info.emplace_back(
						m_storage_descriptor_buffer.address() + alligned_offset * num_storage_descriptors,
						vk::BufferUsageFlagBits::eShaderDeviceAddress |
							vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

					num_storage_descriptors++;
				}

				std::memcpy(memcpy_destination, descriptor_data.data(), descriptor_data.size());
			}
		}

		auto descriptor_buffer::bind(const vk::raii::CommandBuffer& command_buffer,
									 const vk::raii::PipelineLayout& pipeline_layout) const -> void
		{
			auto combined_descriptor_bindings = m_uniform_descriptor_buffer_binding_info;
			combined_descriptor_bindings.insert_range(combined_descriptor_bindings.end(),
													  m_storage_descriptor_buffer_binding_info);
			combined_descriptor_bindings.insert_range(combined_descriptor_bindings.end(),
													  m_combined_image_sampler_descriptor_buffer_binding_info);
			combined_descriptor_bindings.insert_range(combined_descriptor_bindings.end(),
													  m_global_light_descriptor_buffer.light_storage_bindings());

			command_buffer.bindDescriptorBuffersEXT(combined_descriptor_bindings);

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
												combined_image_sampler_descriptor_index,
												light_storage_descriptor_index};

			command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 0, indices, {0, 0, 0, 0});
		}
	}
}
