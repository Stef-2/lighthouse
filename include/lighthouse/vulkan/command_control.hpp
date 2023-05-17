#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;
		class queue_families;

		// e1m4
		class command_control : public vk_wrapper<vk::raii::CommandPool>
		{
		public:
			struct create_info
			{
				vk::CommandPoolCreateFlags m_pool_flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

				vk::CommandBufferLevel m_buffer_level = vk::CommandBufferLevel::ePrimary;
				decltype(vk::CommandBufferAllocateInfo::commandBufferCount) m_num_buffers = {1};
			};

			command_control(const vulkan::logical_device&, const vulkan::queue_families&, const create_info& = {});

			auto command_buffers() const -> const vk::raii::CommandBuffers&;

		private:
			vk::raii::CommandBuffers m_buffers;
		};
	}
}