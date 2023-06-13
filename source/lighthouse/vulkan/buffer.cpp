#include "lighthouse/vulkan/buffer.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"

lh::vulkan::buffer::buffer(const physical_device& physical_device,
						   const logical_device& logical_device,
						   const memory_allocator& allocator,
						   const vk::DeviceSize& size,
						   const create_info& create_info)
	: m_allocation_info {}, m_address {}, m_size {size}, m_create_info {create_info}
{
	const auto buffer_info = vk::BufferCreateInfo({}, size, create_info.m_usage);

	const auto allocation_create_info = vma::AllocationCreateInfo {create_info.m_allocation_flags,
																   vma::MemoryUsage::eAuto,
																   create_info.m_properties};

	auto [buffer, allocation] = allocator->createBuffer(buffer_info, allocation_create_info, &m_allocation_info);

	m_allocation = allocation;
	m_object = {*logical_device, buffer};
	m_address = m_object.getDevice().getBufferAddress(*m_object);
}

auto lh::vulkan::buffer::allocation() const -> const vma::Allocation&
{
	return m_allocation;
}

auto lh::vulkan::buffer::allocation_info() const -> const vma::AllocationInfo&
{
	return m_allocation_info;
}

auto lh::vulkan::buffer::allocation_info() -> vma::AllocationInfo&
{
	return m_allocation_info;
}

auto lh::vulkan::buffer::address() const -> const vk::DeviceAddress&
{
	return m_address;
}

auto lh::vulkan::buffer::size() const -> const vk::DeviceSize&
{
	return m_size;
}

auto lh::vulkan::buffer::create_information() const -> const create_info&
{
	return m_create_info;
}

lh::vulkan::mapped_buffer::mapped_buffer(const physical_device& physical_device,
										 const logical_device& logical_device,
										 const memory_allocator& allocator,
										 const vk::DeviceSize& size,
										 const mapped_buffer::create_info& create_info)
	: buffer(physical_device,
			 logical_device,
			 allocator,
			 size,
			 buffer::create_info {.m_usage = create_info.m_usage,
								  .m_allocation_flags = create_info.m_allocation_flags,
								  .m_properties = create_info.m_properties})

{}
