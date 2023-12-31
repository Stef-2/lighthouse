module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module logical_device;

import lighthouse_string;
import vulkan_string;
import raii_wrapper;
import physical_device;
import queue_families;
import queue;

export namespace lh
{
	namespace vulkan
	{

		class logical_device : public raii_wrapper<vk::raii::Device>
		{
		public:
			struct create_info
			{
				vk_string_t m_extensions {};
			};

			logical_device(const physical_device&, const queue_families&, const create_info& = {});

			auto graphics_queue() const -> const queue&;
			auto present_queue() const -> const queue&;
			auto compute_queue() const -> const queue&;
			auto transfer_queue() const -> const queue&;

			auto info() const -> lh::string::string_t override;

		private:
			std::vector<queue> m_queues;
			std::uint8_t m_present_queue_index;
			std::uint8_t m_compute_queue_index;
			std::uint8_t m_transfer_queue_index;
		};
	}
}
