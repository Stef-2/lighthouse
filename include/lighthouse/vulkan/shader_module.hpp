#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		class shader_module : public vk_wrapper<vk::raii::ShaderModule>
		{
		public:
			struct create_info
			{};

			shader_module();

		private:
		};
	}
}
