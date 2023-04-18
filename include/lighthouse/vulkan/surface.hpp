#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	// forward declarations
	class window;
	namespace vulkan
	{
		class instance;
		class physical_device;
	}

	namespace vulkan
	{
		class surface : public vk_wrapper<vk::raii::SurfaceKHR>
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
