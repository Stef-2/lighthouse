#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;
		class surface;
		class queue_families;
		class memory_allocator;
		class depth_buffer;

		class swapchain : public vk_wrapper<vk::raii::SwapchainKHR>
		{
		public:
			struct create_info
			{
				vk::SurfaceFormat2KHR m_format = {{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}};
				vk::PresentModeKHR m_present_mode = vk::PresentModeKHR::eImmediate;
				uint32_t m_image_count = {2};
				vk::ImageUsageFlagBits m_image_usage = vk::ImageUsageFlagBits::eColorAttachment;
				vk::SharingMode m_sharing_mode = vk::SharingMode::eExclusive;
				vk::SurfaceTransformFlagBitsKHR m_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
				vk::CompositeAlphaFlagBitsKHR m_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
				vk::ImageViewType m_image_view_type = vk::ImageViewType::e2D;
				vk::ImageAspectFlagBits m_image_aspect = vk::ImageAspectFlagBits::eColor;
			} static inline const m_defaults;

			swapchain(const physical_device&,
					  const logical_device&,
					  const surface&,
					  const queue_families&,
					  const memory_allocator&,
					  const create_info& = m_defaults);

		private:
			const surface& m_surface;

			std::vector<image> m_images;
			depth_buffer m_depth_buffer;
		};
	}
}
