#include "lighthouse/vulkan/surface.hpp"
#include "lighthouse/vulkan/instance.hpp"

#include "lighthouse/window.hpp"

lh::vulkan::surface::surface(const lh::window& window, const instance& instance, const create_info& create_info)
	: m_extent {window.resolution()}
{
	auto surface = VkSurfaceKHR {};
	glfwCreateWindowSurface(static_cast<VkInstance>(**instance), window.vkfw_window(), nullptr, &surface);

	m_object = {*instance, surface};
}
