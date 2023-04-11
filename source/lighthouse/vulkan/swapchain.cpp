#include "lighthouse/vulkan/swapchain.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/surface.hpp"
#include "lighthouse/vulkan/queue_families.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"
#include "lighthouse/vulkan/image.hpp"

lh::vulkan::swapchain::swapchain(const vulkan::physical_device& physical_device,
								 const vulkan::logical_device& logical_device,
								 const vulkan::surface& surface,
								 const vulkan::queue_families& queue_families,
								 const vulkan::memory_allocator& memory_allocator,
								 const create_info& create_info)
	: m_images {}, m_depth_buffer(physical_device, logical_device, memory_allocator, surface), m_surface {surface}
{}
