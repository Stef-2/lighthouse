module;
#pragma once

#include <compare>

export module queue;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/queue_families.ixx"
#else
import logical_device;
import queue_families;
import vulkan;
#endif

export namespace lh
{
	namespace vulkan
	{
		class queue
		{

		public:
			struct create_info
			{};

			queue(const logical_device&, const queue_families&, const create_info& = {});

			auto graphics() const -> const vk::raii::Queue&;
			auto present() const -> const vk::raii::Queue&;

		private:
			vk::raii::Queue m_graphics;
			vk::raii::Queue m_present;
		};
	}
}
