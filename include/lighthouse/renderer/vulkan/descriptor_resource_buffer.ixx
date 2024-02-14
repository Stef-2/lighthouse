module;

#if INTELLISENSE
#include "vulkan.hpp"

#include <cstdint>
#include <utility>
#include <vector>
#include <concepts>
#endif

export module descriptor_resource_buffer;

#if not INTELLISENSE
import std.core;
import vulkan_hpp;
#endif

import physical_device;
import logical_device;
import memory_allocator;
import buffer;

export namespace lh
{
	namespace vulkan
	{
		class descriptor_resource_buffer
		{
		public:
			using binding_slot_t = std::uint16_t;
			using descriptor_data_t = std::vector<std::byte>;

			struct create_info
			{
				using binding_type_and_subdata_t = std::pair<vk::DescriptorType, buffer_subdata::subdata>;

				mapped_buffer::create_info m_buffer_create_info = {};
				std::vector<binding_type_and_subdata_t> m_subdata = {};
			};

			descriptor_resource_buffer();
			descriptor_resource_buffer(const physical_device&,
									   const logical_device&,
									   const memory_allocator&,
									   const vk::DeviceSize&,
									   const create_info& = {});

			descriptor_resource_buffer(const descriptor_resource_buffer&) = delete;
			auto operator=(const descriptor_resource_buffer&) = delete;
			descriptor_resource_buffer& operator=(descriptor_resource_buffer&&) noexcept;
			auto operator==(const descriptor_resource_buffer&) const -> bool;

			template <typename T>
			requires(not std::is_pointer_v<T>)
			auto map_uniform_data(const binding_slot_t& binding, const T& data) const
			{
				m_data_buffer.map_data(data,
									   m_uniform_buffer_subdata.m_subdata[binding].m_offset,
									   m_uniform_buffer_subdata.m_subdata[binding].m_size);
			}

			template <typename T>
			requires(not std::is_pointer_v<T>)
			auto map_storage_data(const binding_slot_t& binding, const T& data) const
			{
				m_data_buffer.map_data(data,
									   m_storage_buffer_subdata.m_subdata[binding].m_offset,
									   m_storage_buffer_subdata.m_subdata[binding].m_size);
			}

			auto mapped_buffer() const -> const vulkan::mapped_buffer&;
			auto uniform_descriptors() const -> const std::vector<descriptor_data_t>&;
			auto storage_descriptors() const -> const std::vector<descriptor_data_t>&;

		private:
			vulkan::mapped_buffer m_data_buffer;
			buffer_subdata m_uniform_buffer_subdata;
			buffer_subdata m_storage_buffer_subdata;
			std::vector<descriptor_data_t> m_uniform_descriptors;
			std::vector<descriptor_data_t> m_storage_descriptors;
		};
	}
}