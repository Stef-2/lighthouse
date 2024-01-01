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
			  m_wait_semaphores {},
			  m_wait_destination_stage_mask {},
			  m_signal_semaphores {}
		{}

		auto queue::add_wait_semaphore(const vk::PipelineStageFlags& wait_destination_stage_mask) -> void
		{
			m_wait_semaphores.emplace_back(m_fence.getDevice(), vk::SemaphoreCreateInfo {});
			m_wait_destination_stage_mask.emplace_back(wait_destination_stage_mask);
		}

		auto queue::add_signal_semaphore() -> void
		{
			m_signal_semaphores.emplace_back(m_fence.getDevice(), vk::SemaphoreCreateInfo {});
		}

		auto queue::submit_and_wait(const vk::raii::CommandBuffer& command_buffer) -> void
		{
			const auto submit_info = vk::SubmitInfo {{}, {}, {*command_buffer}, {}};
			m_object.submit(submit_info, *m_fence);

			std::ignore = command_buffer.getDevice().waitForFences(*m_fence, true, fence_timeout);

			command_buffer.getDevice().resetFences(*m_fence);
		}

		auto queue::fence() const -> const vk::raii::Fence&
		{
			return m_fence;
		}
	}
}
