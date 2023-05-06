#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;

		class buffer : public vk_wrapper<vk::raii::Buffer>
		{
		public:
			struct create_info
			{
				vk::BufferUsageFlags m_usage = {};
				vk::MemoryPropertyFlags m_properties = vk::MemoryPropertyFlagBits::eHostVisible |
													   vk::MemoryPropertyFlagBits::eHostCoherent;
			};

			buffer(const physical_device&,
				   const logical_device&,
				   const vma::Allocator&,
				   const vk::DeviceSize&,
				   const create_info& = {});

			auto view() const -> const vk::raii::BufferView&;
			auto memory() const -> const vk::raii::DeviceMemory&;

			template <typename T>
			auto data(const T& data,
					  const vk::raii::DeviceMemory& memory,
					  const std::size_t& count,
					  const vk::DeviceSize& stride = sizeof(T))
			{
				uint8_t* deviceData = static_cast<uint8_t*>(memory.mapMemory(0, count * stride));

				if (stride == sizeof(T))
					memcpy(deviceData, &data, count * sizeof(T));

				else
					for (std::size_t i {}; i < count; i++)
					{
						memcpy(deviceData, &data[i], sizeof(T));
						deviceData += stride;
					}

				memory.unmapMemory();
			}

		private:
			vk::raii::DeviceMemory m_memory;
			vk::raii::BufferView m_view;
		};
	}
}
