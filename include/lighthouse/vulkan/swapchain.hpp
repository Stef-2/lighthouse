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
			using image_index_t = decltype(m_object.acquireNextImage({}).second);
			using image_timeout_t = uint64_t;

			struct create_info
			{
				decltype(vk::SwapchainCreateInfoKHR::minImageCount) m_image_count = {2};
				vk::ImageUsageFlagBits m_image_usage = vk::ImageUsageFlagBits::eColorAttachment;
				vk::SharingMode m_sharing_mode = vk::SharingMode::eExclusive;
				vk::SurfaceTransformFlagBitsKHR m_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
				vk::CompositeAlphaFlagBitsKHR m_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
				vk::ImageViewType m_image_view_type = vk::ImageViewType::e2D;
				vk::ImageAspectFlagBits m_image_aspect = vk::ImageAspectFlagBits::eColor;

				vk::ClearColorValue m_clear_color = {0.5f, 0.5f, 0.5f, 0.5f};
				vk::ClearDepthStencilValue m_clear_depth_stencil = {1.0f, 1u};

				image_timeout_t m_next_image_timeout = 100000000;
			};

			swapchain(const physical_device&,
					  const logical_device&,
					  const surface&,
					  const queue_families&,
					  const memory_allocator&,
					  const create_info& = {});

			auto surface() const -> const surface&;
			auto views() const -> const std::vector<vk::raii::ImageView>&;
			auto depth_stencil_buffer() const -> const image&;
			auto transition_layout_for_rendering(const vk::raii::CommandBuffer&) const -> void;
			auto transition_layout_for_presentation(const vk::raii::CommandBuffer&) const -> void;
			auto next_image_info(const vk::raii::CommandBuffer&, const vk::raii::Semaphore&)
				-> const std::tuple<vk::Result, image_index_t, vk::RenderingInfo>;

		private:
			const vulkan::surface& m_surface;

			std::vector<vk::raii::ImageView> m_views;
			image m_depth_stencil_buffer;

			image_index_t m_current_image_index;
			image_timeout_t m_next_image_timeout;

			vk::RenderingAttachmentInfo m_color_attachment;
			vk::RenderingAttachmentInfo m_depth_stencil_attachment;
		};
	}
}
