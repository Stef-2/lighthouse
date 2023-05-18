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

				vk::BufferUsageFlags m_usage = {};
				vma::AllocationCreateFlags m_allocation_flags = {vma::AllocationCreateFlagBits::eMapped};
				vk::MemoryPropertyFlags m_properties = vk::MemoryPropertyFlagBits::eHostVisible |
													   vk::MemoryPropertyFlagBits::eHostCoherent;
			};

			using vk_wrapper::vk_wrapper;

			buffer(const physical_device&,
				   const logical_device&,
				   const memory_allocator&,
				   const vk::DeviceSize&,
				   const create_info& = {});

			auto memory() const -> const vk::raii::DeviceMemory&;
			auto address() const -> const vk::DeviceAddress;

			template <typename T>
			auto map_data(const T& data, const std::size_t& count = 1, const vk::DeviceSize& stride = sizeof(T)) const
			{
				auto deviceData = static_cast<uint8_t*>(m_memory.mapMemory(0, count * stride));

				if (stride == sizeof(T))
					std::memcpy(deviceData, &data, count * sizeof(T));

				else
					for (std::size_t i {}; i < count; i++)
					{
						std::memcpy(deviceData, &data[i], sizeof(T));
						deviceData += stride;
					}

				m_memory.unmapMemory();
			}

		protected:
			vk::raii::DeviceMemory m_memory;
			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;
		};

		class mapped_buffer : public buffer
		{
		public:
			mapped_buffer(const physical_device&,
						  const logical_device&,
						  const memory_allocator&,
						  const vk::DeviceSize&,
						  const buffer::create_info& = {.m_allocation_flags = {vma::AllocationCreateFlagBits::eMapped},
														.m_properties = vk::MemoryPropertyFlagBits::eHostVisible |
																		vk::MemoryPropertyFlagBits::eHostCoherent});

		private:
		};
	}
}
