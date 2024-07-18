module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif

export module vertex_input_description;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import std;

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