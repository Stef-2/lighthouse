module;

#if INTELLISENSE
#include <cstdint>
#include <vector>
#endif

export module descriptor_resource_buffer;

#if not INTELLISENSE
import std.core;
import vulkan;
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
				mapped_buffer::create_info m_buffer_create_info = {};
				std::vector<buffer_subdata::subdata> m_subdata = {};
			};

			descriptor_resource_buffer();
			descriptor_resource_buffer(const physical_device&,
									   const logical_device&,
									   const memory_allocator&,
									   const vk::DeviceSize&,
									   const create_info& = {});

			descriptor_resource_buffer(const descriptor_resource_buffer&) = delete;
			auto operator=(const descriptor_resource_buffer&) = delete;

			template <typename T>
			requires(not std::is_pointer_v<T>)
			auto map_binding_data(const binding_slot_t& binding, const T& data)
			{
				m_data_buffer.map_data(data, m_buffer_subdata[binding].m_offset, m_buffer_subdata[binding].m_size);
			}

			auto mapped_buffer() const -> const vulkan::mapped_buffer&;
			auto descriptor() const -> const std::vector<descriptor_data_t>&;
		private:
			vulkan::mapped_buffer m_data_buffer;
			buffer_subdata m_buffer_subdata;
			std::vector<descriptor_data_t> m_descriptor;
		};
	}
}