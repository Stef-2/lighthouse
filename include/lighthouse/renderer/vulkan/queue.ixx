module;

#include <compare>

export module queue;

import logical_device;
import queue_families;
import vulkan;

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
