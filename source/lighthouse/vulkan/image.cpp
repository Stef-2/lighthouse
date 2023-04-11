#include "lighthouse/vulkan/image.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"
#include "lighthouse/vulkan/surface.hpp"

lh::vulkan::image::image(const vulkan::physical_device& physical_device,
						 const vulkan::logical_device& logical_device,
						 const vulkan::memory_allocator& memory_allocator,
						 const vulkan::surface& surface,
						 const create_info& create_info)
	: m_format(create_info.m_format), m_view {nullptr}, m_memory {nullptr}
{
	const auto image_info = vk::ImageCreateInfo {create_info.m_image_create_flags,
												 create_info.m_image_type,
												 create_info.m_format,
												 vk::Extent3D(surface.area().extent, 1),
												 1,
												 1,
												 create_info.m_image_sample_count,
												 create_info.m_image_tiling,
												 create_info.m_image_usage | vk::ImageUsageFlagBits::eSampled,
												 create_info.m_image_sharing_mode,
												 {},
												 create_info.m_image_layout};

	const auto allocation_create_info = vma::AllocationCreateInfo {{}, vma::MemoryUsage::eAuto};
	auto allocation_info = vma::AllocationInfo {};

	auto [image, allocation] = memory_allocator->createImage(image_info, allocation_create_info, allocation_info);

	m_object = {*logical_device, image};

	const auto view_info = vk::ImageViewCreateInfo {
		{}, *m_object, create_info.m_view_type, create_info.m_format, {}, {create_info.m_image_aspect, 0, 1, 0, 1}};

	m_view = {*logical_device, view_info};
	m_memory = {*logical_device, allocation_info.deviceMemory};
}

auto lh::vulkan::image::format() const -> const vk::Format&
{
	return m_format;
}

auto lh::vulkan::image::view() const -> const vk::raii::ImageView&
{
	return m_view;
}

auto lh::vulkan::image::memory() const -> const vk::raii::DeviceMemory&
{
	return m_memory;
}

lh::vulkan::depth_buffer::depth_buffer(const vulkan::physical_device& physical_device,
									   const vulkan::logical_device& logical_device,
									   const vulkan::memory_allocator& memory_allocator,
									   const vulkan::surface& surface,
									   const create_info& create_info)
	: image(physical_device, logical_device, memory_allocator, surface, create_info)
{}
