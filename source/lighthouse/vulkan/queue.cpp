#include "lighthouse/vulkan/queue.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/queue_families.hpp"

lh::vulkan::queue::queue(const logical_device& logical_device,
						 const queue_families& queue_families,
						 const create_info& create_info)
	: m_graphics {*logical_device, queue_families.graphics_index(), 0},
	  m_present {*logical_device, queue_families.present_index(), 0}
{}

auto lh::vulkan::queue::graphics() const -> const vk::raii::Queue&
{
	return m_graphics;
}

auto lh::vulkan::queue::present() const -> const vk::raii::Queue&
{
	return m_present;
}
