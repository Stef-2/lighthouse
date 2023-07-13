module;
#pragma once

#include "lighthouse/renderer/vulkan/logical_device.hpp"
#include "lighthouse/renderer/vulkan/queue_families.hpp"

export module command_control;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#else
import raii_wrapper;
#endif

export namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;
		class queue_families;

		// e1m4
		class command_control : public raii_wrapper<vk::raii::CommandPool>
		{
		public:
			struct create_info
			{
				vk::CommandPoolCreateFlags m_pool_flags = {};
				vk::CommandBufferUsageFlags m_usage_flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

				vk::CommandBufferLevel m_buffer_level = vk::CommandBufferLevel::ePrimary;
				decltype(vk::CommandBufferAllocateInfo::commandBufferCount) m_num_buffers = {1};
			};

			command_control(const vulkan::logical_device&, const vulkan::queue_families&, const create_info& = {});

			auto command_buffers() const -> const vk::raii::CommandBuffers&;
			auto first_command_buffer() const -> const vk::raii::CommandBuffer&;
			auto usage_flags() const -> const vk::CommandBufferUsageFlags&;
			auto reset() const -> void;

		private:
			vk::raii::CommandBuffers m_buffers;
			vk::CommandBufferUsageFlags m_usage_flags;
		};
	}
}
