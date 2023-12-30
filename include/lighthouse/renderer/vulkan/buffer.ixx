module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#include <cstddef>
#include <cstring>
#endif

export module buffer;

import lighthouse_utility;
import raii_wrapper;
import logical_device;
import memory_allocator;

export namespace lh
{
	namespace vulkan
	{
		class buffer : public raii_wrapper<vk::raii::Buffer>
		{
		public:
			using raii_wrapper::raii_wrapper;

			using used_memory_percentage_t = float;

			struct create_info
			{
				vk::BufferUsageFlags m_usage = {};
				vk::MemoryPropertyFlags m_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
				vma::AllocationCreateInfo m_allocation_create_info = {{},
																	  vma::MemoryUsage::eAuto,
																	  m_memory_properties,
																	  m_memory_properties};
			};

			buffer(const logical_device&, const memory_allocator&, const vk::DeviceSize&, const create_info& = {});

			auto allocation_info() const -> const vma::AllocationInfo&;
			auto allocation_info() -> vma::AllocationInfo&;
			auto allocation() const -> const vma::Allocation&;

			auto address() const -> const vk::DeviceAddress&;
			auto size() const -> const vk::DeviceSize&;
			auto used_memory() const -> const vk::DeviceSize&;
			auto remaining_memory() const -> const vk::DeviceSize&;
			auto used_memory_percentage() const -> const used_memory_percentage_t;
			auto create_information() const -> const create_info&;

		protected:
			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;

			vk::DeviceAddress m_address;
			vk::DeviceSize m_used_memory;

			create_info m_create_info;
		};

		class mapped_buffer : public buffer
		{
		public:
			using buffer::buffer;

			struct create_info
			{
				vk::BufferUsageFlags m_usage = {vk::BufferUsageFlagBits::eShaderDeviceAddress};
				vk::MemoryPropertyFlags m_memory_properties = {vk::MemoryPropertyFlagBits::eHostVisible |
														vk::MemoryPropertyFlagBits::eHostCoherent};
				vma::AllocationCreateInfo m_allocation_create_info = {vma::AllocationCreateFlagBits::eMapped,
																	  vma::MemoryUsage::eAuto,
																	  m_memory_properties,
																	  m_memory_properties};
			};

			mapped_buffer(const logical_device&,
						  const memory_allocator&,
						  const vk::DeviceSize&,
						  const mapped_buffer::create_info& = {});
			~mapped_buffer();
			mapped_buffer(mapped_buffer&&) = default;
			mapped_buffer& operator=(mapped_buffer&&) = default;

			auto mapped_data_pointer() const -> void*;
			auto map() -> void;
			auto unmap() -> void;

			template <typename T>
			requires (not std::is_pointer_v<T>)
			auto map_data(const T& data, const std::size_t& offset = 0, const std::size_t& size = sizeof(T)) const
			{
				const auto destination = static_cast<std::byte*>(m_allocation_info.pMappedData) + offset;

				std::memcpy(destination, &data, size);
			}

		private:
			const memory_allocator* m_allocator;
			void* m_mapped_data_pointer;
		};

		struct buffer_subdata
		{
			struct subdata
			{
				vk::DeviceAddress m_offset;
				vk::DeviceSize m_size;
			};

			auto operator[](std::size_t) const -> const subdata&;

			lh::non_owning_ptr<mapped_buffer> m_buffer;
			std::vector<subdata> m_subdata;
		};
	}
}