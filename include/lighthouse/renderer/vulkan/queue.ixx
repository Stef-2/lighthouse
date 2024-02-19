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
			using fence_timeout_t = std::uint64_t;

			struct create_info
			{
				queue_families::family m_queue_family;
				fence_timeout_t m_fence_timeout = 100000000/*std::numeric_limits<fence_timeout_t>::max()*/;
				command_control::create_info m_command_control_create_info {};
			};
			
			enum class queue_state
			{
				initial,
				recording,
				executing
			};

			queue(const logical_device&, const create_info& = {});

			//auto add_wait_semaphore(const vk::PipelineStageFlags&, const vk::raii::Semaphore&) -> void;
			auto wait_semaphores() const -> const std::vector<vk::raii::Semaphore*>&;
			auto add_signal_semaphore() -> void;
			auto wait() -> void;
			auto submit_and_wait() -> void;

			auto command_control() const -> const vulkan::command_control&;
			auto record_commands() -> const vk::raii::CommandBuffer&;
			auto fence() const -> const vk::raii::Fence&;
			auto queue_state() const -> const queue::queue_state&;

		protected:
			auto clear() -> void;

			const logical_device& m_logical_device;

			decltype(queue_state::initial) m_queue_state;
			vulkan::command_control m_command_control;

			fence_timeout_t m_fence_timeout;
			vk::raii::Fence m_fence;

			std::vector<vk::raii::Semaphore*> m_wait_semaphores;
			std::vector<vk::PipelineStageFlags> m_wait_destination_stage_masks;

			std::vector<vk::raii::Semaphore> m_signal_semaphores;
		};

		class graphics_queue : public queue
		{
		public:
			graphics_queue(const logical_device&, const swapchain&, const create_info& = {});

			auto present() const -> void;

		private:
			const swapchain& m_swapchain;
			std::vector<vk::Semaphore*> m_present_semaphores;
		};
	}
}