module;

#include <compare>

module queue;

namespace lh
{
	namespace vulkan
	{

		queue::queue(const logical_device& logical_device, const create_info& create_info)
			: raii_wrapper {{*logical_device, create_info.m_create_info}},
			  m_fence {*logical_device, {}},
			  m_semaphores {}
		{}

		auto queue::fence() const -> const vk::raii::Fence&
		{
			return m_fence;
		}

		auto queue::semaphores() -> std::vector<vk::raii::Semaphore>&
		{
			return m_semaphores;
		}

	}
}
