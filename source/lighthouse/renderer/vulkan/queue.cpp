module;

#include <compare>

module queue;

namespace lh
{
	namespace vulkan
	{

		queue::queue(const logical_device& logical_device, const create_info& create_info)
			: raii_wrapper {{*logical_device, {{}, create_info.m_queue_family.m_index, 0}}},
			  m_logical_device {logical_device},
			  m_command_control {logical_device, create_info.m_queue_family, create_info.m_command_control_create_info},
			  m_fence {*logical_device, vk::FenceCreateInfo {}},
			  m_wait_semaphores {},
			  m_wait_destination_stage_masks {},
			  m_signal_semaphores {}
		{}

		auto queue::add_wait_semaphore(const vk::PipelineStageFlags& wait_destination_stage_mask) -> void
		{
			m_wait_semaphores.emplace_back(m_logical_device, vk::SemaphoreCreateInfo {});
			m_wait_destination_stage_masks.emplace_back(wait_destination_stage_mask);
		}

		auto queue::wait_semaphores() const -> const std::vector<vk::raii::Semaphore>&
		{
			return m_wait_semaphores;
		}

		auto queue::add_signal_semaphore() -> void
		{
			m_signal_semaphores.emplace_back(m_logical_device, vk::SemaphoreCreateInfo {});
		}

		auto queue::submit_and_wait() -> void
		{
			const auto submit_info = vk::SubmitInfo {{}, {}, *m_command_control.front(), {}};
			m_object.submit(submit_info, *m_fence);

			std::ignore = m_logical_device->waitForFences(*m_fence, true, fence_timeout);

			clear();
		}

		auto queue::present(const swapchain& swapchain) const -> void
		{
			std::ignore = m_object.presentKHR(
				{*m_wait_semaphores.back(), **swapchain, swapchain.current_image_index()});
		}

		auto queue::command_control() const -> const vulkan::command_control&
		{
			return m_command_control;
		}

		auto queue::fence() const -> const vk::raii::Fence&
		{
			return m_fence;
		}

		auto queue::clear() -> void
		{
			m_logical_device->resetFences(*m_fence);
			m_wait_semaphores.clear();
			m_wait_destination_stage_masks.clear();
			m_signal_semaphores.clear();
		}
	}
}
