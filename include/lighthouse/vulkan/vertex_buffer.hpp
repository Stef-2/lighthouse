#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

#include <vector>
#include <pair>

namespace lh
{
	namespace vulkan
	{
		class vertex_buffer
		{
		public:
			using vertex_input_t =
				std::pair<vk::VertexInputBindingDescription2EXT, std::vector<vk::VertexInputAttributeDescription2EXT>>;

		private:
		};
	}
}
