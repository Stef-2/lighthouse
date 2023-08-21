module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module surface;

import raii_wrapper;
import window;
import instance;
import physical_device;

export namespace lh
{
	namespace vulkan
	{
		class surface : public raii_wrapper<vk::raii::SurfaceKHR>
		{
		public:
			struct create_info
			{
				vk::SurfaceFormat2KHR m_format = {{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}};
				vk::PresentModeKHR m_present_mode = vk::PresentModeKHR::eImmediate;
			};

			surface(const lh::window&, const instance&, const physical_device&, const create_info& = {});

			auto extent() const -> const vk::Extent2D&;
			auto format() const -> const vk::SurfaceFormat2KHR&;
			auto capabilities() const -> const vk::SurfaceCapabilities2KHR&;
			auto present_mode() const -> const vk::PresentModeKHR&;

		private:
			vk::Extent2D m_extent;
			vk::SurfaceCapabilities2KHR m_capabilities;
			vk::PresentModeKHR m_present_mode;
			vk::SurfaceFormat2KHR m_format;
		};
	}
}
