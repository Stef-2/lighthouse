#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;
		class memory_allocator;

		class buffer : public vk_wrapper<vk::raii::Buffer>
		{
		public:
			struct create_info
			{
				vk::BufferUsageFlags m_usage = {vk::BufferUsageFlagBits::eShaderDeviceAddress};
				vma::AllocationCreateFlags m_allocation_flags = {};
				vk::MemoryPropertyFlags m_properties = {};
			};

			buffer(const physical_device&,
				   const logical_device&,
				   const memory_allocator&,
				   const vk::DeviceSize&,
				   const create_info& = {});

			auto allocation_info() const -> const vma::AllocationInfo&;
			auto allocation_info() -> vma::AllocationInfo&;
			auto allocation() const -> const vma::Allocation&;
			auto address() const -> const vk::DeviceAddress&;

		protected:
			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;
			vk::DeviceAddress m_address;
		};

		class mapped_buffer : public buffer
		{
		public:
			struct create_info
			{
				vk::BufferUsageFlags m_usage = {};
				vma::AllocationCreateFlags m_allocation_flags = {vma::AllocationCreateFlagBits::eMapped};
				vk::MemoryPropertyFlags m_properties = {vk::MemoryPropertyFlagBits::eHostVisible |
														vk::MemoryPropertyFlagBits::eHostCoherent};
			};

			mapped_buffer(const physical_device&,
						  const logical_device&,
						  const memory_allocator&,
						  const vk::DeviceSize&,
						  const mapped_buffer::create_info& = {});

			template <typename T>
			auto map_data(const T& data, const std::size_t& count = 1, const vk::DeviceSize& stride = sizeof(T)) const
			{
				auto map = static_cast<uint8_t*>(m_allocation_info.pMappedData);

				if (stride == sizeof(T))
					std::memcpy(map, &data, count * sizeof(T));

				else
					for (std::size_t i {}; i < count; i++)
					{
						std::memcpy(map, &data[i], sizeof(T));
						map += stride;
					}
			}

		private:
		};
	}
}
