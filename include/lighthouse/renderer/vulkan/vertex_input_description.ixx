module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <vector>
#endif

export module vertex_input_description;

#if not INTELLISENSE
import vulkan;
import std.core;
#endif

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