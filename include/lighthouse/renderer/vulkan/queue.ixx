module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module queue;

import raii_wrapper;
import logical_device;
import queue_families;

#if not INTELLISENSE
import vulkan;
#endif

export namespace lh
{
	namespace vulkan
	{
		class queue : public raii_wrapper<vk::raii::Queue>
		{

		public:
			struct create_info
			{
				vk::DeviceQueueInfo2 m_create_info {};
			};
			
			queue(const logical_device&, const create_info& = {});

			auto fence() const -> const vk::raii::Fence&;
			auto semaphores() -> std::vector<vk::raii::Semaphore>&;

		private:
			vk::raii::Fence m_fence;
			std::vector<vk::raii::Semaphore> m_semaphores;
		};
	}
}