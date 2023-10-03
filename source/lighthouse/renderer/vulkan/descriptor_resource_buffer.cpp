module;

#if INTELLISENSE
#include "vulkan_raii.hpp"
#endif

module descriptor_resource_buffer;

import vulkan_utility;

namespace lh
{
	namespace vulkan
	{
		descriptor_resource_buffer::descriptor_resource_buffer()
			: m_data_buffer {}, m_buffer_subdata {}, m_descriptor {}
		{}

		descriptor_resource_buffer::descriptor_resource_buffer(const physical_device& physical_device,
															   const logical_device& logical_device,
															   const memory_allocator& memory_allocator,
															   const vk::DeviceSize& buffer_size,
															   const create_info& create_info)
			: m_data_buffer {logical_device, memory_allocator, buffer_size, create_info.m_buffer_create_info},
			  m_buffer_subdata {&m_data_buffer, create_info.m_subdata},
			  m_descriptor {}
		{
			const auto& descriptor_buffer_properties = physical_device.properties().m_descriptor_buffer_properties;
			const auto binding_offset = descriptor_buffer_properties.m_uniform_buffer_offset;

			m_descriptor.reserve(create_info.m_subdata.size());

			for (auto i = binding_slot_t {}; const auto& subdata : m_buffer_subdata.m_subdata)
			{
				m_descriptor.emplace_back();
				m_descriptor.back().resize(descriptor_buffer_properties.m_uniform_buffer_size);

				const auto data_address_info = vk::DescriptorAddressInfoEXT {m_buffer_subdata.m_buffer->address() +
																				 subdata.m_offset,
																			 subdata.m_size};

				void* data_address = static_cast<std::byte*>(m_data_buffer.allocation_info().pMappedData) +
									 i * binding_offset;

				logical_device->getDescriptorEXT({vk::DescriptorType::eUniformBuffer, {&data_address_info}},
												 descriptor_buffer_properties.m_properties.uniformBufferDescriptorSize,
												 m_descriptor.back().data());
			}
		}

		auto descriptor_resource_buffer::mapped_buffer() const -> const vulkan::mapped_buffer&
		{
			return m_data_buffer;
		}

		auto descriptor_resource_buffer::descriptor() const -> const std::vector<descriptor_data_t>&
		{
			return m_descriptor;
		}
	}
}
