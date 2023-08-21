module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module command_control;

import raii_wrapper;
import logical_device;
import queue_families;

export namespace lh
{
	namespace vulkan
	{
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
