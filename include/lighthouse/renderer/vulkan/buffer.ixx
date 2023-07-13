module;
#pragma once

export module buffer;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/utility.ixx"
#else
import utility;
import raii_wrapper;
#endif

export namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;
		class memory_allocator;

		class buffer : public raii_wrapper<vk::raii::Buffer>
		{
		public:
			using raii_wrapper::raii_wrapper;

			struct create_info
			{
				vk::BufferUsageFlags m_usage = {};
				vma::AllocationCreateFlags m_allocation_flags = {};
				vk::MemoryPropertyFlags m_properties = {};
			};

			buffer(const logical_device&, const memory_allocator&, const vk::DeviceSize&, const create_info& = {});

			auto allocation_info() const -> const vma::AllocationInfo&;
			auto allocation_info() -> vma::AllocationInfo&;
			auto allocation() const -> const vma::Allocation&;

			auto address() const -> const vk::DeviceAddress&;
			auto size() const -> const vk::DeviceSize&;
			auto create_information() const -> const create_info&;

		protected:
			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;

			vk::DeviceAddress m_address;
			vk::DeviceSize m_size;

			create_info m_create_info;
		};

		class mapped_buffer : public buffer
		{
		public:
			using buffer::buffer;

			struct create_info
			{
				vk::BufferUsageFlags m_usage = {vk::BufferUsageFlagBits::eShaderDeviceAddress};
				vma::AllocationCreateFlags m_allocation_flags = {vma::AllocationCreateFlagBits::eMapped};
				vk::MemoryPropertyFlags m_properties = {vk::MemoryPropertyFlagBits::eHostVisible |
														vk::MemoryPropertyFlagBits::eHostCoherent};
			};

			mapped_buffer(const logical_device&,
						  const memory_allocator&,
						  const vk::DeviceSize&,
						  const mapped_buffer::create_info& = {});

			template <typename T>
			auto map_data(const T& data, const std::size_t& offset = 0, const std::size_t& size = sizeof(T)) const
			{
				const auto map = static_cast<uint8_t*>(m_allocation_info.pMappedData) + offset;

				std::memcpy(map, &data, size);
			}

		private:
		};

		struct buffer_subdata
		{
			struct subdata
			{
				vk::DeviceAddress m_offset;
				vk::DeviceSize m_size;
			};

			non_owning_ptr<mapped_buffer> m_buffer;
			std::vector<subdata> m_subdata;
		};
	}
}
