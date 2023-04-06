#include "lighthouse/vulkan/surface.hpp"
#include "lighthouse/vulkan/instance.hpp"

#include "lighthouse/window.hpp"

lh::vulkan::surface::surface(const lh::window& window, const instance& instance, const create_info& create_info)
	: m_extent {window.resolution()}, vk_wrapper {std::invoke([&instance, &window] {
		  auto surface = VkSurfaceKHR {};
		  glfwCreateWindowSurface(static_cast<VkInstance>(**instance), window.vkfw_window(), nullptr, &surface);

		  return vk::raii::SurfaceKHR {*instance, surface};
	  })}
{}

auto lh::vulkan::surface::extent() const -> const vk::Extent2D&
{
	return m_extent;
}