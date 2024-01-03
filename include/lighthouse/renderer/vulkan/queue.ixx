module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module queue;

import raii_wrapper;
import logical_device;
import queue_families;
import command_control;
import swapchain;

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
				queue_families::family m_queue_family;
				command_control::create_info m_command_control_create_info {};
			};
			
			queue(const logical_device&, const create_info& = {});

			auto add_wait_semaphore(const vk::PipelineStageFlags&) -> void;
			auto wait_semaphores() const -> const std::vector<vk::raii::Semaphore>&;
			auto add_signal_semaphore() -> void;
			auto submit_and_wait() -> void;
			auto present(const swapchain&) const -> void;

			auto command_control() const -> const vulkan::command_control&;
			auto fence() const -> const vk::raii::Fence&;

		private:
			auto clear() -> void;

			const logical_device& m_logical_device;

			vulkan::command_control m_command_control;
			vk::raii::Fence m_fence;
			std::vector<vk::raii::Semaphore> m_wait_semaphores;
			std::vector<vk::PipelineStageFlags> m_wait_destination_stage_masks;

			std::vector<vk::raii::Semaphore> m_signal_semaphores;
		};
	}
}