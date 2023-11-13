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
					   const vk::DeviceSize& size,
					   const create_info& create_info)
			: m_allocation_info {}, m_allocation {}, m_address {}, m_size {size}, m_create_info {create_info}
		{
			const auto buffer_info = vk::BufferCreateInfo({}, size, m_create_info.m_usage);

			auto [buffer, allocation] = allocator->createBuffer(buffer_info,
																m_create_info.m_allocation_create_info,
																&m_allocation_info);

			m_allocation = allocation;
			m_object = {*logical_device, buffer};
			m_address = logical_device->getBufferAddress(*m_object);
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
			return m_size;
		}

		auto buffer::create_information() const -> const create_info&
		{
			return m_create_info;
		}

		mapped_buffer::mapped_buffer(const logical_device& logical_device,
									 const memory_allocator& allocator,
									 const vk::DeviceSize& size,
									 const mapped_buffer::create_info& create_info)
			: buffer(logical_device,
					 allocator,
					 size,
					 buffer::create_info {.m_usage = create_info.m_usage,
										  .m_memory_properties = create_info.m_memory_properties,
										  .m_allocation_create_info = create_info.m_allocation_create_info}),
			  m_mapped_data_pointer {m_allocation_info.pMappedData}

		{}

		auto mapped_buffer::mapped_data_pointer() const -> void*
		{
			return m_mapped_data_pointer;
		}

		auto buffer_subdata::operator[](std::size_t index) const -> const buffer_subdata::subdata&
		{
			return m_subdata[index];
		}
	}
}
