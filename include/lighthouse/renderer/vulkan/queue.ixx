module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <cstdint>
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
				fence_timeout_t m_fence_timeout = std::numeric_limits<fence_timeout_t>::max();
				command_control::create_info m_command_control_create_info {};
			};
			
			struct semaphore
			{
				vk::Semaphore m_semaphore = {};
				std::uint64_t m_semaphore_value = 0;
				vk::PipelineStageFlags2 m_pipeline_stage = {};
			};

			enum class queue_state
			{
				initial,
				recording,
				executing
			};

			queue(const logical_device&, const create_info& = {});

			auto add_submit_wait_semaphore(const semaphore&) -> void;
			auto add_submit_signal_semaphore(const semaphore&) -> void;
			auto submit() -> void;
			auto submit_and_wait() -> void;

			auto command_control() const -> const vulkan::command_control&;
			auto record_commands() -> const vk::raii::CommandBuffer&;
			auto queue_state() const -> const queue::queue_state&;

		protected:
			virtual auto clear() -> void;

			const logical_device& m_logical_device;

			decltype(queue_state::initial) m_queue_state;
			vulkan::command_control m_command_control;

			fence_timeout_t m_fence_timeout;
			vk::raii::Fence m_submit_fence;

			std::vector<vk::SemaphoreSubmitInfo> m_submit_wait_semaphores;
			std::vector<vk::SemaphoreSubmitInfo> m_submit_signal_semaphores;
		};

		class graphics_queue : public queue
		{
		public:
			graphics_queue(const logical_device&, const swapchain&, const create_info& = {});

			auto add_present_wait_semaphore(const vk::Semaphore&) -> void;
			auto present() const -> void;
			auto present_and_wait() -> void;

		private:
			auto clear() -> void override;

			const swapchain& m_swapchain;

			vk::raii::Fence m_present_fence;
			std::vector<vk::Semaphore> m_present_wait_semaphores;
		};
	}
}