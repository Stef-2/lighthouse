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
			  m_submit_fence {*logical_device, vk::FenceCreateInfo {/*vk::FenceCreateFlagBits::eSignaled*/}},
			  m_submit_wait_semaphores {},
			  m_submit_signal_semaphores {}
		{}

		auto queue::add_submit_wait_semaphore(const semaphore& semaphore) -> void
		{
			m_submit_wait_semaphores.emplace_back(semaphore.m_semaphore,
												  semaphore.m_semaphore_value,
												  semaphore.m_pipeline_stage);
		}

		auto queue::add_submit_signal_semaphore(const semaphore& semaphore) -> void
		{
			m_submit_signal_semaphores.emplace_back(semaphore.m_semaphore,
													semaphore.m_semaphore_value,
													semaphore.m_pipeline_stage);
		}

		auto queue::submit() -> void
		{
			const auto command_buffer_submit_info = m_command_control.command_buffer_submit_info();

			const auto submit_info =
				vk::SubmitInfo2 {{}, m_submit_wait_semaphores, command_buffer_submit_info, m_submit_signal_semaphores};

			m_object.submit2(submit_info, *m_submit_fence);
			m_queue_state = queue_state::executing;
		}

		auto queue::submit_and_wait() -> void
		{
			submit();

			std::ignore = m_logical_device->waitForFences(*m_submit_fence, true, m_fence_timeout);

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

		auto queue::queue_state() const -> const decltype(queue_state::initial)&
		{
			return m_queue_state;
		}

		auto queue::clear() -> void
		{
			m_logical_device->resetFences(*m_submit_fence);
			m_submit_wait_semaphores.clear();
			m_submit_signal_semaphores.clear();
			m_queue_state = queue_state::initial;
		}
	}

	vulkan::graphics_queue::graphics_queue(const logical_device& logical_device,
										   const swapchain& swapchain,
										   const create_info& create_info)
		: queue {logical_device, create_info},
		  m_swapchain {swapchain},
		  m_present_fence {*logical_device, vk::FenceCreateInfo {}},
		  m_present_wait_semaphores {}
	{}

	auto vulkan::graphics_queue::add_present_wait_semaphore(const vk::Semaphore& semaphore) -> void
	{
		m_present_wait_semaphores.emplace_back(semaphore);
	}

	auto vulkan::graphics_queue::present() const -> void
	{
		const auto present_fence_info = vk::SwapchainPresentFenceInfoEXT {*m_present_fence};

		std::ignore = m_object.presentKHR(
			{m_present_wait_semaphores, **m_swapchain, m_swapchain.current_image_index(), {}, &present_fence_info});
	}

	auto vulkan::graphics_queue::present_and_wait() -> void
	{
		present();

		std::ignore = m_logical_device->waitForFences(*m_present_fence, true, m_fence_timeout);

		clear();
	}

	auto vulkan::graphics_queue::clear() -> void
	{
		queue::clear();

		m_logical_device->resetFences(*m_present_fence);
		m_present_wait_semaphores.clear();
	}
}
