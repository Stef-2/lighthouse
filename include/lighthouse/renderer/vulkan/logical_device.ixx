module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module logical_device;

import lighthouse_string;
import vulkan_string;
import raii_wrapper;
import physical_device;
import queue_families;

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

			auto dispatcher() const -> const vk::raii::DeviceDispatcher*;
			auto info() const -> lh::string::string_t override;

		private:
		};
	}
}