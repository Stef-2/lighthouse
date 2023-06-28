#pragma once

#include <vector>

namespace lh
{
	namespace vulkan
	{
		struct vertex_input_description
		{
			vk::VertexInputBindingDescription2EXT m_bindings;
			std::vector<vk::VertexInputAttributeDescription2EXT> m_attributes;
		};
	}
}
