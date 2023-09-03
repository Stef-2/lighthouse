module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

#include <compare>

export module queue;

import logical_device;
import queue_families;

#if not INTELLISENSE
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
			auto transfer() const -> const vk::raii::Queue&;

		private:
			vk::raii::Queue m_graphics;
			vk::raii::Queue m_present;
			vk::raii::Queue m_transfer;
		};
	}
}
