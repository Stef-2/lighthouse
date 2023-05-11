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
				vk::BufferUsageFlags m_usage = {vk::BufferUsageFlagBits::eUniformBuffer};
				vk::MemoryPropertyFlags m_properties = vk::MemoryPropertyFlagBits::eHostVisible |
													   vk::MemoryPropertyFlagBits::eHostCoherent;
			};

			buffer(const physical_device&,
				   const logical_device&,
				   const memory_allocator&,
				   const vk::DeviceSize&,
				   const create_info& = {});

			auto memory() const -> const vk::raii::DeviceMemory&;

			template <typename T>
			auto map_data(const T& data, const std::size_t& count = 1, const vk::DeviceSize& stride = sizeof(T))
			{
				auto deviceData = static_cast<uint8_t*>(m_memory.mapMemory(0, count * stride));

				if (stride == sizeof(T))
					memcpy(deviceData, &data, count * sizeof(T));

				else
					for (std::size_t i {}; i < count; i++)
					{
						memcpy(deviceData, &data[i], sizeof(T));
						deviceData += stride;
					}

				m_memory.unmapMemory();
			}

		private:
			vk::raii::DeviceMemory m_memory;
		};
	}
}
