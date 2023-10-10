module;

#if INTELLISENSE
#include "vulkan_raii.hpp"
#endif
#include "glm/vec4.hpp"
module descriptor_resource_buffer;

import vulkan_utility;

namespace lh
{
	auto descriptor_offset_and_size(const lh::vulkan::physical_device& physical_device,
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
			: m_data_buffer {}, m_buffer_subdata {}, m_descriptors {}
		{}

		descriptor_resource_buffer::descriptor_resource_buffer(const physical_device& physical_device,
															   const logical_device& logical_device,
															   const memory_allocator& memory_allocator,
															   const vk::DeviceSize& buffer_size,
															   const create_info& create_info)
			: m_data_buffer {logical_device, memory_allocator, buffer_size, create_info.m_buffer_create_info},
			  m_buffer_subdata {&m_data_buffer},
			  m_descriptors {}
		{
			const auto descriptor_buffer_properties =
				physical_device.properties().m_descriptor_buffer_properties.m_properties;
			m_descriptors.reserve(create_info.m_subdata.size());

			for (auto combined_offset = vk::DeviceSize {};
				 const auto& [descriptor_type, subdata] : create_info.m_subdata)
			{
				const auto [binding_offset, binding_size] = descriptor_offset_and_size(physical_device,
																					   descriptor_type);

				const auto descriptor_size = descriptor_type == vk::DescriptorType::eUniformBuffer
												 ? descriptor_buffer_properties.uniformBufferDescriptorSize
												 : descriptor_buffer_properties.storageBufferDescriptorSize;

				m_buffer_subdata.m_subdata.push_back(subdata);

				m_descriptors.emplace_back();
				m_descriptors.back().first = descriptor_type;
				m_descriptors.back().second.resize(descriptor_size);

				const auto data_address_info = vk::DescriptorAddressInfoEXT {
					m_buffer_subdata.m_buffer->address() + m_buffer_subdata.m_subdata.back().m_offset,
					m_buffer_subdata.m_subdata.back().m_size};

				logical_device->getDescriptorEXT({descriptor_type, {&data_address_info}},
												 descriptor_size,
												 m_descriptors.back().second.data());

				combined_offset += binding_offset;
			}
		}

		descriptor_resource_buffer& descriptor_resource_buffer::operator=(descriptor_resource_buffer&& other) noexcept
		{
			m_data_buffer = std::move(other.m_data_buffer);
			m_buffer_subdata = std::move(other.m_buffer_subdata);
			m_buffer_subdata.m_buffer = &m_data_buffer;
			m_descriptors = std::move(other.m_descriptors);

			other.m_data_buffer = {};
			other.m_buffer_subdata = {};
			other.m_descriptors = {};

			return *this;
		}

		auto descriptor_resource_buffer::mapped_buffer() const -> const vulkan::mapped_buffer&
		{
			return m_data_buffer;
		}

		auto descriptor_resource_buffer::descriptors() const -> const std::vector<descriptor_data_t>&
		{
			return m_descriptors;
		}
		auto descriptor_resource_buffer::subdata() const -> const buffer_subdata&
		{
			return m_buffer_subdata;
		}
	}
}
