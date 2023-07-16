module;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/surface.ixx"
#else
module surface;
#endif

lh::vulkan::surface::surface(const lh::window& window,
							 const instance& instance,
							 const physical_device& physical_device,
							 const create_info& create_info)
	: m_extent {window.resolution()},
	  raii_wrapper {std::invoke([&instance, &window] {
		  auto surface = VkSurfaceKHR {};
		  glfwCreateWindowSurface(static_cast<VkInstance>(**instance), window.vkfw_window(), nullptr, &surface);

		  return vk::raii::SurfaceKHR {*instance, surface};
	  })},
	  m_capabilities {physical_device->getSurfaceCapabilities2KHR(*m_object)},
	  m_present_mode {create_info.m_present_mode},
	  m_format {create_info.m_format}
{
	const auto& vk_physical_device = *physical_device;
	const auto formats = vk_physical_device.getSurfaceFormats2KHR(*m_object);
	const auto present_modes = vk_physical_device.getSurfacePresentModesKHR(*m_object);

	// attempt to acquire the prefered surface format, if unavailable, take the first one that is
	if (!std::ranges::contains(formats, create_info.m_format))
	{
		output::warning() << "this system does not support the prefered vulkan surface format";
		m_format = formats.front();
	}

	// attempt to acquire the prefered present mode, if unavailable, default to FIFO
	if (!std::ranges::contains(present_modes, create_info.m_present_mode))
	{
		output::warning() << "this system does not support the prefered vulkan present mode";
		m_present_mode = vk::PresentModeKHR::eFifo;
	}
}

auto lh::vulkan::surface::extent() const -> const vk::Extent2D&
{
	return m_extent;
}

auto lh::vulkan::surface::format() const -> const vk::SurfaceFormat2KHR&
{
	return m_format;
}

auto lh::vulkan::surface::capabilities() const -> const vk::SurfaceCapabilities2KHR&
{
	return m_capabilities;
}

auto lh::vulkan::surface::present_mode() const -> const vk::PresentModeKHR&
{
	return m_present_mode;
}
