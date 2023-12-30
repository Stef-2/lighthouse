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

export namespace lh
{
	namespace vulkan
	{

		class logical_device : public raii_wrapper<vk::raii::Device>
		{
		public:
			struct create_info
			{
				std::vector<vk::DeviceQueueCreateInfo> m_queues {};

				vk_string_t m_extensions {};
			};

			logical_device(const physical_device&, const create_info&);

			auto info() const -> lh::string::string_t override;

		private:
			std::vector<vk::raii::Queue> m_queues;
		};
	}
}
