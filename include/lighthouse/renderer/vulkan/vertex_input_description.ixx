module;
#pragma once

#include <vector>

export module vertex_input_description;

export namespace lh
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
