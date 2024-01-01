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
			static inline constexpr auto fence_timeout = 1'000'000'000;

			struct create_info
			{
				vk::DeviceQueueInfo2 m_create_info {};
			};
			
			queue(const logical_device&, const create_info& = {});

			auto add_wait_semaphore(const vk::PipelineStageFlags&) -> void;
			auto add_signal_semaphore() -> void;
			auto submit_and_wait(const vk::raii::CommandBuffer&) -> void;

			auto fence() const -> const vk::raii::Fence&;

		private:
			vk::raii::Fence m_fence;
			std::vector<vk::raii::Semaphore> m_wait_semaphores;
			std::vector<vk::PipelineStageFlags> m_wait_destination_stage_mask;

			std::vector<vk::raii::Semaphore> m_signal_semaphores;
		};
	}
}