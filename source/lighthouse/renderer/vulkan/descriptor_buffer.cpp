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
		descriptor_buffer::descriptor_buffer()
			: m_descriptor_buffer_binding_info {},
			  m_physical_device {},
			  m_logical_device {},
			  m_descriptor_set_layout {},
			  m_bind_point {vk::PipelineBindPoint::eGraphics},
			  m_basic_uniform_buffer {nullptr},
			  m_basic_combined_image_sampler {nullptr},
			  m_resource_descriptor_buffer {},
			  m_combined_image_sampler_descriptor_buffer {}
		{}

		descriptor_buffer::descriptor_buffer(const physical_device& physical_device,
											 const logical_device& logical_device,
											 const memory_allocator& memory_allocator,
											 const descriptor_set_layout& descriptor_set_layout,
											 const create_info& create_info)
			: m_descriptor_buffer_binding_info {},
			  m_physical_device {&physical_device},
			  m_logical_device {&logical_device},
			  m_descriptor_set_layout {&descriptor_set_layout},
			  m_bind_point {create_info.m_bind_point},
			  m_basic_uniform_buffer {*logical_device,
									  {vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT,
									   s_basic_uniform_buffer_binding}},
			  m_basic_combined_image_sampler {*logical_device,
											  {vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT,
											   s_basic_combined_image_sampler_binding}},
			  m_resource_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  m_basic_uniform_buffer.getSizeEXT() * create_info.m_num_uniform_buffer_bindings,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT,
											  .m_properties = create_info.m_descriptor_buffer_memory_properties}},
			  m_combined_image_sampler_descriptor_buffer {
				  logical_device,
				  memory_allocator,
				  m_basic_combined_image_sampler.getSizeEXT() * create_info.m_num_combined_image_sampler_bindings,
				  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
														 vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT,
											  .m_properties = create_info.m_descriptor_buffer_memory_properties}}
		{}

		auto descriptor_buffer::map_resource_data(const buffer_subdata& buffer_subdata) -> void
		{
			const auto& descriptor_buffer_properties = m_physical_device->properties().m_descriptor_buffer_properties;

			for (auto i = std::size_t {}; i < m_descriptor_set_layout->bindings().size(); i++)
			{
				const auto& binding = m_descriptor_set_layout->bindings()[i];
				const auto binding_offset = (*m_descriptor_set_layout)->getBindingOffsetEXT(binding.m_binding);

				m_descriptor_buffer_binding_info.emplace_back(
					m_resource_descriptor_buffer.address() +
						i * utility::aligned_size(binding_offset,
												  descriptor_buffer_properties.descriptorBufferOffsetAlignment),
					descriptor_buffer_usage(*m_descriptor_set_layout));

				const auto data_address_info = vk::DescriptorAddressInfoEXT {buffer_subdata.m_buffer->address() +
																				 buffer_subdata.m_subdata[i].m_offset,
																			 buffer_subdata.m_subdata[i].m_size};

				const auto& descriptor_info = static_cast<VkDescriptorGetInfoEXT>(
					vk::DescriptorGetInfoEXT {binding.m_type, {&data_address_info}});

				(*m_logical_device)
					->getDispatcher()
					->vkGetDescriptorEXT(***m_logical_device,
										 &descriptor_info,
										 descriptor_buffer::descriptor_size(*m_physical_device, binding.m_type),
										 static_cast<std::byte*>(
											 m_resource_descriptor_buffer.allocation_info().pMappedData) +
											 binding_offset);
			}
		}

		auto descriptor_buffer::map_uniform_buffer_data(const binding_slot_t& offset,
														const buffer_subdata& buffer_subdata) -> void
		{
			const auto& descriptor_buffer_properties = m_physical_device->properties().m_descriptor_buffer_properties;
			const auto descriptor_offset = m_basic_uniform_buffer.getBindingOffsetEXT(1);
			const auto binding_slot_offset = offset * descriptor_offset;

			for (auto i = binding_slot_t {}; i < buffer_subdata.m_subdata.size(); i++)
			{
				m_descriptor_buffer_binding_info.emplace_back(
					m_resource_descriptor_buffer.address() +
						i * utility::aligned_size(descriptor_offset,
												  descriptor_buffer_properties.descriptorBufferOffsetAlignment) +
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

		auto descriptor_buffer::map_texture_data(const std::vector<const texture&>& textures) -> void {}

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
			command_buffer.bindDescriptorBuffersEXT(m_descriptor_buffer_binding_info);
			command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 0, {0}, {0});
		}

		auto descriptor_buffer::descriptor_size(const physical_device& physical_device,
												const vk::DescriptorType& descriptor_type) -> const std::size_t
		{
			const auto& descriptor_properties = physical_device.properties().m_descriptor_buffer_properties;

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

		auto descriptor_buffer::descriptor_buffer_usage(const descriptor_set_layout& descriptor_set_layout)
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
	}
}
