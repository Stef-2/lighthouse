module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module texture;

#if not INTELLISENSE
import vulkan;
import std.core;
#endif

namespace lh
{
	class texture
	{
	public:

	private:
		vk::raii::Image m_image;
		vk::raii::ImageView m_view;
	};
}