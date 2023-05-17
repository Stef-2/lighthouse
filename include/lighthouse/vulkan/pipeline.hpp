#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		class pipeline ::public vk_wrapper<vk::raii::pipeline>
		{
		public:
			struct create_info
			{};

			pipeline();

		private:
		};
	}
}
