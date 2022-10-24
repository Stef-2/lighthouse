#include "Renderer.hpp"

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

#include "vulkan.hpp"
#include "vulkan_raii.hpp"

namespace lh
{
	class Renderer
	{
	public:

	private:

		vk::raii::SurfaceKHR vkSurface;
		vk::raii::Instance vkInstance;
	};
}