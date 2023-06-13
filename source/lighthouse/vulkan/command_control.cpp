#include "lighthouse/vulkan/command_control.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/queue_families.hpp"

lh::vulkan::command_control::command_control(const vulkan::logical_device& logical_device,
											 const vulkan::queue_families& queue_families,
											 const create_info& create_info)
	: m_buffers {nullptr}, m_usage_flags {create_info.m_usage_flags}
{
	m_object = {*logical_device, {create_info.m_pool_flags, queue_families.graphics().m_index}};

	auto command_buffers_info = vk::CommandBufferAllocateInfo {*m_object,
															   create_info.m_buffer_level,
															   create_info.m_num_buffers};
	m_buffers = {*logical_device, command_buffers_info};
}

auto lh::vulkan::command_control::command_buffers() const -> const vk::raii::CommandBuffers&
{
	return m_buffers;
}

auto lh::vulkan::command_control::first_command_buffer() const -> const vk::raii::CommandBuffer&
{
	return m_buffers.front();
}

auto lh::vulkan::command_control::usage_flags() const -> const vk::CommandBufferUsageFlags&
{
	return m_usage_flags;
}

auto lh::vulkan::command_control::reset() const -> void
{
	m_object.reset(vk::CommandPoolResetFlagBits::eReleaseResources);
}
