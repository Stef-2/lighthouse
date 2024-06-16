module;

#include "vulkan/vulkan.h"
#include "vulkan/vma/vk_mem_alloc.hpp"

module buffer;

namespace lh
{
	namespace vulkan
	{
		buffer::buffer(const logical_device& logical_device,
					   const memory_allocator& allocator,
					   const vk::DeviceSize size,
					   const create_info& create_info)
			: raii_wrapper {nullptr},
			  m_logical_device {&logical_device},
			  m_allocator {&allocator},
			  m_allocation_info {},
			  m_allocation {},
			  m_address {},
			  m_used_memory {},
			  m_create_info {create_info}
		{
			const auto buffer_info = vk::BufferCreateInfo({}, size, m_create_info.m_usage);

			auto [buffer, allocation] =
				(*m_allocator)->createBuffer(buffer_info, m_create_info.m_allocation_create_info, &m_allocation_info);

			m_object = {*logical_device, buffer};
			m_allocation = allocation;
			m_address = create_info.m_usage & vk::BufferUsageFlagBits::eShaderDeviceAddress
							? logical_device->getBufferAddress(*m_object)
							: 0;
		}

		buffer::buffer(buffer&& other) noexcept
			: m_logical_device {std::exchange(other.m_logical_device, nullptr)},
			  m_allocator {std::exchange(other.m_allocator, nullptr)},
			  m_allocation_info {std::exchange(other.m_allocation_info, {})},
			  m_allocation {std::exchange(other.m_allocation, {})},
			  m_address {std::exchange(other.m_address, {})},
			  m_used_memory {std::exchange(other.m_used_memory, {})},
			  m_create_info {std::exchange(other.m_create_info, {})}
		{}

		buffer& buffer::operator=(buffer&& other) noexcept
		{
			m_logical_device = std::exchange(other.m_logical_device, nullptr);
			m_allocator = std::exchange(other.m_allocator, nullptr);
			m_allocation_info = std::exchange(other.m_allocation_info, {});
			m_allocation = std::exchange(other.m_allocation, {});
			m_address = std::exchange(other.m_address, {});
			m_used_memory = std::exchange(other.m_used_memory, {});
			m_create_info = std::exchange(other.m_create_info, {});

			return *this;
		}

		buffer::~buffer()
		{
			if (*m_object and m_allocation) (*m_allocator)->freeMemory(m_allocation);
		}

		auto buffer::allocation() const -> const vma::Allocation&
		{
			return m_allocation;
		}

		auto buffer::allocation_info() const -> const vma::AllocationInfo&
		{
			return m_allocation_info;
		}

		auto buffer::allocation_info() -> vma::AllocationInfo&
		{
			return m_allocation_info;
		}

		auto buffer::address() const -> const vk::DeviceAddress&
		{
			return m_address;
		}

		auto buffer::size() const -> const vk::DeviceSize&
		{
			return m_allocation_info.size;
		}

		auto buffer::used_memory() const -> const vk::DeviceSize&
		{
			return m_used_memory;
		}

		auto buffer::remaining_memory() const -> const vk::DeviceSize
		{
			return m_allocation_info.size - m_used_memory;
		}

		auto buffer::used_memory_percentage() const -> const used_memory_percentage_t
		{
			return static_cast<used_memory_percentage_t>(m_used_memory) / m_allocation_info.size * 100.0f;
		}

		auto buffer::create_information() const -> const create_info&
		{
			return m_create_info;
		}

		mapped_buffer::mapped_buffer(const logical_device& logical_device,
									 const memory_allocator& allocator,
									 const vk::DeviceSize size,
									 const create_info& create_info)
			: buffer(logical_device,
					 allocator,
					 size,
				create_info
					 /*buffer::create_info {.m_usage = create_info.m_usage,
										  .m_memory_properties = create_info.m_memory_properties,
										  .m_allocation_create_info = create_info.m_allocation_create_info}*/),
			  m_mapped_data_pointer {m_allocation_info.pMappedData}

		{}

		mapped_buffer::~mapped_buffer()
		{
			unmap();
		}

		mapped_buffer::mapped_buffer(mapped_buffer&& other) noexcept
			: buffer(std::move(other.buffer)), m_mapped_data_pointer {std::exchange(other.m_mapped_data_pointer, {})}
		{}

		auto mapped_buffer::mapped_data_pointer() const -> void*
		{
			return m_mapped_data_pointer;
		}

		auto mapped_buffer::map() -> void
		{
			m_mapped_data_pointer = (*m_allocator)->mapMemory(m_allocation);
		}

		auto mapped_buffer::unmap() -> void
		{
			if (m_allocation_info.pMappedData)
			{
				m_mapped_data_pointer = nullptr;
				(*m_allocator)->unmapMemory(m_allocation);
			}
		}

		template <typename T>
		mapped_buffer_span<T>::mapped_buffer_span(const logical_device& logical_device,
												  const memory_allocator& memory_allocator,
												  const vk::DeviceSize size,
												  const mapped_buffer::create_info& create_info)
			: mapped_buffer {logical_device, memory_allocator, size, create_info},
			  memory_mapped_span<T> {this->m_mapped_data_pointer, m_allocation_info.size / sizeof T}
		{}
	}
}
