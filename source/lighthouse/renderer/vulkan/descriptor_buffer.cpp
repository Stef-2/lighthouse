module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif
// #include "vulkan/vulkan.h"

module descriptor_buffer;

import output;

namespace lh
{
	namespace vulkan
	{
		descriptor_buffer::descriptor_buffer(const physical_device& physical_device,
											 const logical_device& logical_device,
											 const memory_allocator& memory_allocator,
											 const create_info& create_info)
			: m_resource_descriptor_buffer_binding_info {},
			  m_combined_image_sampler_descriptor_buffer_binding_info {},
			  m_physical_device {&physical_device},
			  m_logical_device {&logical_device},
			  m_bind_point {create_info.m_bind_point},
			  m_resource_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  static_cast<vk::DeviceSize>(
					  physical_device.properties().m_descriptor_buffer_properties.m_uniform_buffer_size) *
					  create_info.m_num_uniform_buffer_bindings,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties = create_info.m_descriptor_buffer_memory_properties}},
			  m_combined_image_sampler_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  static_cast<vk::DeviceSize>(
					  physical_device.properties().m_descriptor_buffer_properties.m_combined_image_sampler_size) *
					  create_info.m_num_combined_image_sampler_bindings,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT,
											  .m_memory_properties = create_info.m_descriptor_buffer_memory_properties}}
		{}

		descriptor_buffer::descriptor_buffer(const physical_device& physical_device,
											 const logical_device& logical_device,
											 const memory_allocator& memory_allocator,
											 const global_descriptor& global_descriptor,
											 const create_info& create_info)
			: m_resource_descriptor_buffer_binding_info {},
			  m_combined_image_sampler_descriptor_buffer_binding_info {},
			  m_physical_device {&physical_device},
			  m_logical_device {&logical_device},
			  m_bind_point {create_info.m_bind_point},
			  m_resource_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  global_descriptor.uniform_buffer_set().getSizeEXT() +
					  global_descriptor.storage_descriptor_set().getSizeEXT(),
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_memory_properties = create_info.m_descriptor_buffer_memory_properties}},
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
			const auto& descriptor_buffer_properties = m_physical_device->properties().m_descriptor_buffer_properties;
			const auto descriptor_offset = descriptor_buffer_properties.m_uniform_buffer_offset;
			const auto binding_slot_offset = offset * descriptor_offset;

			for (auto i = binding_slot_t {}; i < buffer_subdata.m_subdata.size(); i++)
			{
				m_resource_descriptor_buffer_binding_info.emplace_back(
					m_resource_descriptor_buffer.address() +
						i * utility::aligned_size(
								static_cast<vk::DeviceSize>(descriptor_offset),
								descriptor_buffer_properties.m_memory_properties.descriptorBufferOffsetAlignment) +
						i * binding_slot_offset,
					vk::BufferUsageFlagBits::eShaderDeviceAddress |
						vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

				const auto data_address_info = vk::DescriptorAddressInfoEXT {buffer_subdata.m_buffer->address() +
																				 buffer_subdata.m_subdata[i].m_offset,
																			 buffer_subdata.m_subdata[i].m_size};

				const auto& descriptor_info = static_cast<VkDescriptorGetInfoEXT>(
					vk::DescriptorGetInfoEXT {vk::DescriptorType::eUniformBuffer, {&data_address_info}});

				(*m_logical_device)
					->getDispatcher()
					->vkGetDescriptorEXT(
						***m_logical_device,
						&descriptor_info,
						descriptor_buffer::descriptor_size(*m_physical_device, vk::DescriptorType::eUniformBuffer),
						static_cast<std::byte*>(m_resource_descriptor_buffer.allocation_info().pMappedData) +
							i * descriptor_offset + binding_slot_offset);
			}
		}

		auto descriptor_buffer::map_texture_data(const std::vector<texture*>& textures) -> void
		{
			const auto& descriptor_buffer_properties = m_physical_device->properties().m_descriptor_buffer_properties;
			const auto descriptor_offset = descriptor_buffer_properties.m_combined_image_sampler_offset;

			for (auto i = binding_slot_t {}; i < textures.size(); i++)
			{
				m_combined_image_sampler_descriptor_buffer_binding_info.emplace_back(
					m_combined_image_sampler_descriptor_buffer.address() +
						i * utility::aligned_size(
								static_cast<vk::DeviceSize>(descriptor_offset),
								descriptor_buffer_properties.m_memory_properties.descriptorBufferOffsetAlignment),
					vk::BufferUsageFlagBits::eShaderDeviceAddress |
						vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT);

				const auto combined_image_sampler_data = vk::DescriptorImageInfo {
					**textures[i]->sampler(),
					*textures[i]->image().view(),
					textures[i]->image().create_information().m_image_create_info.initialLayout};

				const auto& descriptor_info = static_cast<VkDescriptorGetInfoEXT>(
					vk::DescriptorGetInfoEXT {vk::DescriptorType::eCombinedImageSampler,
											  {&combined_image_sampler_data}});

				(*m_logical_device)
					->getDispatcher()
					->vkGetDescriptorEXT(***m_logical_device,
										 &descriptor_info,
										 descriptor_buffer::descriptor_size(*m_physical_device,
																			vk::DescriptorType::eCombinedImageSampler),
										 static_cast<std::byte*>(
											 m_combined_image_sampler_descriptor_buffer.allocation_info().pMappedData) +
											 i * descriptor_offset);
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
			auto combined_descriptor_buffer_binding_info = std::vector<vk::DescriptorBufferBindingInfoEXT> {};
			combined_descriptor_buffer_binding_info.reserve(
				m_resource_descriptor_buffer_binding_info.size() +
				m_combined_image_sampler_descriptor_buffer_binding_info.size());
			combined_descriptor_buffer_binding_info.insert_range(combined_descriptor_buffer_binding_info.end(),
																 m_resource_descriptor_buffer_binding_info);
			combined_descriptor_buffer_binding_info.insert_range(
				combined_descriptor_buffer_binding_info.end(), m_combined_image_sampler_descriptor_buffer_binding_info);

			command_buffer.bindDescriptorBuffersEXT(combined_descriptor_buffer_binding_info);
			command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 0, {0}, {0});

			// command_buffer.bindDescriptorBuffersEXT(m_combined_image_sampler_descriptor_buffer_binding_info);
			command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 2, {2}, {0});
		}

		auto descriptor_buffer::descriptor_size(const physical_device& physical_device,
												const vk::DescriptorType& descriptor_type) -> const std::size_t
		{
			const auto& descriptor_properties =
				physical_device.properties().m_descriptor_buffer_properties.m_memory_properties;

			switch (descriptor_type)
			{
				case vk::DescriptorType::eUniformBuffer: return descriptor_properties.uniformBufferDescriptorSize;
				case vk::DescriptorType::eStorageBuffer: return descriptor_properties.storageBufferDescriptorSize;
				case vk::DescriptorType::eSampler: return descriptor_properties.samplerDescriptorSize;
				case vk::DescriptorType::eCombinedImageSampler:
					return descriptor_properties.combinedImageSamplerDescriptorSize;
				default: break;
			}

			std::unreachable();
		}
	}
}
