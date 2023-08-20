module;

#include <compare>

module queue;

namespace lh
{
	namespace vulkan
	{

		queue::queue(const logical_device& logical_device,
					 const queue_families& queue_families,
					 const create_info& create_info)
			: m_graphics {*logical_device, queue_families.graphics().m_index, 0},
			  m_present {*logical_device, queue_families.present().m_index, 0}
		{}

		auto queue::graphics() const -> const vk::raii::Queue&
		{
			return m_graphics;
		}

		auto queue::present() const -> const vk::raii::Queue&
		{
			return m_present;
		}
	}
}
