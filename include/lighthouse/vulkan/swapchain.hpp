#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"
#include "lighthouse/vulkan/image.hpp"

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

		class swapchain : public vk_wrapper<vk::raii::SwapchainKHR>
		{
		public:
			struct create_info
			{
				uint32_t m_image_count = {2};
				vk::ImageUsageFlagBits m_image_usage = vk::ImageUsageFlagBits::eColorAttachment;
				vk::SharingMode m_sharing_mode = vk::SharingMode::eExclusive;
				vk::SurfaceTransformFlagBitsKHR m_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
				vk::CompositeAlphaFlagBitsKHR m_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
				vk::ImageViewType m_image_view_type = vk::ImageViewType::e2D;
				vk::ImageAspectFlagBits m_image_aspect = vk::ImageAspectFlagBits::eColor;
			};

			swapchain(const physical_device&,
					  const logical_device&,
					  const surface&,
					  const queue_families&,
					  const memory_allocator&,
					  const create_info& = {});

			auto surface() const -> const surface&;
			auto views() const -> const std::vector<vk::raii::ImageView>&;
			auto depth_buffer() const -> const depth_buffer&;

		private:
			const vulkan::surface& m_surface;

			std::vector<vk::raii::ImageView> m_views;
			vulkan::depth_buffer m_depth_buffer;
		};
	}
}
