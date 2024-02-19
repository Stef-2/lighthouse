module;

#include "vkfw/vkfw.hpp"

#include "Winuser.h"

module surface;
import output;

namespace lh
{
	namespace vulkan
	{
		surface::surface(const lh::window& window,
						 const instance& instance,
						 const physical_device& physical_device,
						 const create_info& create_info)
			: m_extent {window.resolution()},
			  raii_wrapper {std::invoke([&instance, &window] {
				  auto surface = VkSurfaceKHR {};
				  glfwCreateWindowSurface(static_cast<VkInstance>(**instance), window.vkfw_window(), nullptr, &surface);

				  return vk::raii::SurfaceKHR {*instance, surface};
			  })},
			  m_capabilities {},
			  m_present_mode {create_info.m_present_mode},
			  m_format {create_info.m_format}
		{
			const auto formats = physical_device->getSurfaceFormats2KHR(*m_object);
			const auto present_modes = physical_device->getSurfacePresentModesKHR(*m_object);

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

			// query surface capabilities
			const auto monitor_handle = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
			auto fullscreen_exclusive_info = vk::SurfaceFullScreenExclusiveWin32InfoEXT {monitor_handle};
			auto surface_present_mode = vk::SurfacePresentModeEXT {m_present_mode, &fullscreen_exclusive_info};

			const auto capabilities =
				physical_device->getSurfaceCapabilities2KHR<vk::SurfaceCapabilities2KHR,
															vk::SharedPresentSurfaceCapabilitiesKHR,
															vk::SurfaceCapabilitiesFullScreenExclusiveEXT,
															vk::SurfacePresentScalingCapabilitiesEXT,
															vk::SurfaceProtectedCapabilitiesKHR,
															vk::SurfacePresentModeCompatibilityEXT>(
					{*m_object, &surface_present_mode});

			auto present_mode_compatibility = surface_capabilities::present_mode_compatibility {
				std::get<vk::SurfacePresentModeCompatibilityEXT>(capabilities), {}};
			present_mode_compatibility.m_compatible_present_modes.resize(
				present_mode_compatibility.m_present_mode_compatiblity.presentModeCount);

			const auto full_present_mode_compatibility = vk::SurfacePresentModeCompatibilityEXT {
				static_cast<std::uint32_t>(present_mode_compatibility.m_compatible_present_modes.size()),
				present_mode_compatibility.m_compatible_present_modes.data(),
				&surface_present_mode};
			physical_device->getSurfaceCapabilities2KHR({*m_object, &surface_present_mode});
			present_mode_compatibility.m_present_mode_compatiblity.pPresentModes =
				present_mode_compatibility.m_compatible_present_modes.data();

			m_capabilities = {std::get<vk::SurfaceCapabilities2KHR>(capabilities),
							  std::get<vk::SharedPresentSurfaceCapabilitiesKHR>(capabilities),
							  std::get<vk::SurfaceCapabilitiesFullScreenExclusiveEXT>(capabilities),
							  std::get<vk::SurfacePresentScalingCapabilitiesEXT>(capabilities),
							  std::get<vk::SurfaceProtectedCapabilitiesKHR>(capabilities),
							  present_mode_compatibility};
		}

		auto surface::extent() const -> const vk::Extent2D&
		{
			return m_extent;
		}

		auto surface::format() const -> const vk::SurfaceFormat2KHR&
		{
			return m_format;
		}

		auto surface::capabilities() const -> const surface_capabilities&
		{
			return m_capabilities;
		}

		auto surface::present_mode() const -> const vk::PresentModeKHR&
		{
			return m_present_mode;
		}
	}
}
