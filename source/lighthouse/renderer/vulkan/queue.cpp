module;

#include <compare>

module queue;

import lighthouse_utility;

namespace lh
{
	namespace vulkan
	{

		queue::queue(const logical_device& logical_device, const create_info& create_info)
			: raii_wrapper {{*logical_device, {{}, create_info.m_queue_family.m_index, 0}}},
			  m_logical_device {logical_device},
			  m_queue_state {queue_state::initial},
			  m_command_control {logical_device, create_info.m_queue_family, create_info.m_command_control_create_info},
			  m_fence_timeout {create_info.m_fence_timeout},
			  m_fence {*logical_device, vk::FenceCreateInfo {/*vk::FenceCreateFlagBits::eSignaled*/}},
			  m_wait_semaphores {},
			  m_wait_destination_stage_masks {},
			  m_signal_semaphores {}
		{}
		/*
		auto queue::add_wait_semaphore(const vk::PipelineStageFlags& wait_destination_stage_mask,
									   const vk::raii::Semaphore& semaphore) -> void
		{
			m_wait_semaphores.emplace_back(&semaphore);
			m_wait_destination_stage_masks.emplace_back(wait_destination_stage_mask);
		}
		*/
		auto queue::wait_semaphores() const -> const std::vector<vk::raii::Semaphore*>&
		{
			return m_wait_semaphores;
		}

		auto queue::add_signal_semaphore() -> void
		{
			m_signal_semaphores.emplace_back(m_logical_device, vk::SemaphoreCreateInfo {});
		}

		auto queue::wait() -> void
		{
			std::ignore = m_logical_device->waitForFences(*m_fence, true, m_fence_timeout);

			clear();
		}

		auto queue::submit_and_wait() -> void
		{
			/*
			auto semaphores = std::vector<vk::Semaphore> {};
			semaphores.reserve(m_wait_semaphores.size());
			for (const auto& semaphore : m_wait_semaphores)
				semaphores.emplace_back(**semaphore);*/

			const auto submit_info =
				vk::SubmitInfo {nullptr, nullptr /*m_wait_destination_stage_masks*/, *m_command_control.front(), {}};

			m_object.submit(submit_info, *m_fence);
			m_queue_state = queue_state::executing;

			std::ignore = m_logical_device->waitForFences(*m_fence, true, m_fence_timeout);

			clear();
		}

		auto queue::command_control() const -> const vulkan::command_control&
		{
			return m_command_control;
		}

		auto queue::record_commands() -> const vk::raii::CommandBuffer&
		{
			m_queue_state = m_queue_state == queue_state::executing ? queue_state::executing : queue_state::recording;

			return m_command_control.front();
		}

		auto queue::fence() const -> const vk::raii::Fence&
		{
			return m_fence;
		}

		auto queue::queue_state() const -> const decltype(queue_state::initial)&
		{
			return m_queue_state;
		}

		auto queue::clear() -> void
		{
			m_logical_device->resetFences(*m_fence);
			m_wait_semaphores.clear();
			m_wait_destination_stage_masks.clear();
			m_signal_semaphores.clear();
			m_queue_state = queue_state::initial;
		}
	}

	vulkan::graphics_queue::graphics_queue(const logical_device& logical_device,
										   const swapchain& swapchain,
										   const create_info& create_info)
		: queue {logical_device, create_info}, m_swapchain {swapchain}
	{}

	auto vulkan::graphics_queue::present() const -> void
	{
		std::ignore = m_object.presentKHR(
			{*m_swapchain.current_frame_synchronization_data().m_render_finished_semaphore,
			 **m_swapchain,
			 m_swapchain.current_image_index()});
		/*
		std::ignore = m_logical_device->waitForFences(
			*m_swapchain.current_frame_synchronization_data().m_render_finished_fence, true, m_fence_timeout);*/

		m_logical_device->resetFences(*m_swapchain.current_frame_synchronization_data().m_render_finished_fence);
		m_logical_device->resetFences(*m_fence);
	}
}
