module;

#if INTELLISENSE
#include "vulkan_raii.hpp"
#endif

#include "glm/vec4.hpp"

module descriptor_resource_buffer;

import vulkan_utility;

namespace lh
{
	static auto descriptor_offset_and_size(const lh::vulkan::physical_device& physical_device,
										   const vk::DescriptorType& descriptor)
	{
		const auto& descriptor_type_properties = physical_device.properties().m_descriptor_buffer_properties;

		switch (descriptor)
		{
			case vk::DescriptorType::eUniformBuffer:
				return std::pair {descriptor_type_properties.m_uniform_buffer_offset,
								  descriptor_type_properties.m_uniform_buffer_size};
			case vk::DescriptorType::eStorageBuffer:
				return std::pair {descriptor_type_properties.m_storage_buffer_offset,
								  descriptor_type_properties.m_storage_buffer_size};
			default: std::unreachable();
		}
	}

	namespace vulkan
	{
		descriptor_resource_buffer::descriptor_resource_buffer()
			: m_data_buffer {},
			  m_uniform_buffer_subdata {},
			  m_storage_buffer_subdata {},
			  m_uniform_descriptors {},
			  m_storage_descriptors {}
		{}

		descriptor_resource_buffer::descriptor_resource_buffer(const physical_device& physical_device,
															   const logical_device& logical_device,
															   const memory_allocator& memory_allocator,
															   const vk::DeviceSize& buffer_size,
															   const create_info& create_info)
			: m_data_buffer {logical_device, memory_allocator, buffer_size, create_info.m_buffer_create_info},
			  m_uniform_buffer_subdata {&m_data_buffer},
			  m_storage_buffer_subdata {&m_data_buffer},
			  m_uniform_descriptors {},
			  m_storage_descriptors {}
		{
			const auto& descriptor_buffer_properties =
				physical_device.properties().m_descriptor_buffer_properties.m_properties;

			for (void* descriptor_pointer = nullptr; const auto& [descriptor_type, subdata] : create_info.m_subdata)
			{
				const auto [binding_offset, binding_size] = descriptor_offset_and_size(physical_device,
																					   descriptor_type);

				const auto descriptor_size = descriptor_type == vk::DescriptorType::eUniformBuffer
												 ? descriptor_buffer_properties.uniformBufferDescriptorSize
												 : descriptor_buffer_properties.storageBufferDescriptorSize;

				if (descriptor_type == vk::DescriptorType::eUniformBuffer)
				{
					m_uniform_buffer_subdata.m_subdata.push_back(subdata);
					m_uniform_descriptors.emplace_back();
					m_uniform_descriptors.back().resize(descriptor_size);
					descriptor_pointer = m_uniform_descriptors.back().data();
				} else
				{
					m_storage_buffer_subdata.m_subdata.push_back(subdata);
					m_storage_descriptors.emplace_back();
					m_storage_descriptors.back().resize(descriptor_size);
					descriptor_pointer = m_storage_descriptors.back().data();
				}
				/*
				m_descriptors.emplace_back();
				m_descriptors.back().second.resize(descriptor_size);*/

				const auto data_address_info = vk::DescriptorAddressInfoEXT {
					m_uniform_buffer_subdata.m_buffer->address() + subdata.m_offset, subdata.m_size};

				logical_device->getDescriptorEXT({descriptor_type, {&data_address_info}},
												 descriptor_size,
												 descriptor_pointer);
			}
		}

		descriptor_resource_buffer& descriptor_resource_buffer::operator=(descriptor_resource_buffer&& other) noexcept
		{
			m_data_buffer = std::exchange(other.m_data_buffer, {});

			m_uniform_buffer_subdata = std::exchange(other.m_uniform_buffer_subdata, {});
			m_storage_buffer_subdata = std::exchange(other.m_storage_buffer_subdata, {});

			m_uniform_buffer_subdata.m_buffer = &m_data_buffer;
			m_storage_buffer_subdata.m_buffer = &m_data_buffer;

			m_uniform_descriptors = std::exchange(other.m_uniform_descriptors, {});
			m_storage_descriptors = std::exchange(other.m_storage_descriptors, {});

			return *this;
		}

		auto descriptor_resource_buffer::mapped_buffer() const -> const vulkan::mapped_buffer&
		{
			return m_data_buffer;
		}

		auto descriptor_resource_buffer::uniform_descriptors() const -> const std::vector<descriptor_data_t>&
		{
			return m_uniform_descriptors;
		}

		auto descriptor_resource_buffer::storage_descriptors() const -> const std::vector<descriptor_data_t>&
		{
			return m_storage_descriptors;
		}
	}
}
