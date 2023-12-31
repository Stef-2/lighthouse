module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

module command_control;

namespace lh
{
	namespace vulkan
	{

		command_control::command_control(const logical_device& logical_device,
										 const queue_families::family& queue,
										 const create_info& create_info)
			: raii_wrapper {{*logical_device, {create_info.m_pool_flags, queue.m_index}}},
			  m_buffers {*logical_device, {*m_object, create_info.m_buffer_level, create_info.m_num_buffers}},
			  m_usage_flags {create_info.m_usage_flags}
		{}

		auto command_control::command_buffers() const -> const vk::raii::CommandBuffers&
		{
			return m_buffers;
		}

		auto command_control::front() const -> const vk::raii::CommandBuffer&
		{
			return m_buffers.front();
		}

		auto command_control::usage_flags() const -> const vk::CommandBufferUsageFlags&
		{
			return m_usage_flags;
		}

		auto command_control::reset() const -> void
		{
			m_object.reset(vk::CommandPoolResetFlagBits::eReleaseResources);
		}
	}
}
