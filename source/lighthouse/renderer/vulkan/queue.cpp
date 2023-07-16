module;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/queue.ixx"
#else
module queue;
#endif

lh::vulkan::queue::queue(const logical_device& logical_device,
						 const queue_families& queue_families,
						 const create_info& create_info)
	: m_graphics {*logical_device, queue_families.graphics().m_index, 0},
	  m_present {*logical_device, queue_families.present().m_index, 0}
{}

auto lh::vulkan::queue::graphics() const -> const vk::raii::Queue&
{
	return m_graphics;
}

auto lh::vulkan::queue::present() const -> const vk::raii::Queue&
{
	return m_present;
}
