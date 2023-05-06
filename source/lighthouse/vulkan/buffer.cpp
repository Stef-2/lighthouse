#include "lighthouse/vulkan/buffer.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"

lh::vulkan::buffer::buffer(const physical_device& physical_device,
						   const logical_device& logical_device,
						   const vma::Allocator& allocator,
						   const vk::DeviceSize& size,
						   const create_info& create_info)
	: m_view {nullptr}, m_memory {nullptr}
{
	const auto buffer_info = vk::BufferCreateInfo({}, size, create_info.m_usage);
	const auto allocation_create_info = vma::AllocationCreateInfo {{},
																   vma::MemoryUsage::eAuto,
																   create_info.m_properties};
	auto allocation_info = vma::AllocationInfo {};

	auto [buffer, allocation] = allocator.createBuffer(buffer_info, allocation_create_info, allocation_info);

	m_object = {*logical_device, buffer};
	m_memory = {*logical_device, allocation_info.deviceMemory};
}

auto lh::vulkan::buffer::view() const -> const vk::raii::BufferView&
{
	return m_view;
}

auto lh::vulkan::buffer::memory() const -> const vk::raii::DeviceMemory&
{
	return m_memory;
}
