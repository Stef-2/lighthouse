module;

#if INTELLISENSE
#include <algorithm>
#endif

module swapchain;

namespace lh
{
	namespace vulkan
	{
		swapchain::swapchain(const vulkan::physical_device& physical_device,
							 const vulkan::logical_device& logical_device,
							 const vulkan::surface& surface,
							 const vulkan::queue_families& queue_families,
							 const vulkan::memory_allocator& memory_allocator,
							 const create_info& create_info)
			: m_surface {surface},
			  m_views {},
			  m_depth_stencil_buffer {logical_device,
									  memory_allocator,
									  image::create_info {{{},
														   vk::ImageType::e2D,
														   vk::Format::eD24UnormS8Uint,
														   vk::Extent3D {surface.extent(), 1},
														   1,
														   1,
														   vk::SampleCountFlagBits::e1,
														   vk::ImageTiling::eOptimal,
														   vk::ImageUsageFlagBits::eDepthStencilAttachment,
														   vk::SharingMode::eExclusive}}},
			  m_depth_stencil_view {logical_device,
									{{},
									 **m_depth_stencil_buffer,
									 vk::ImageViewType::e2D,
									 vk::Format::eD24UnormS8Uint,
									 {},
									 {vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil,
									  0,
									  vk::RemainingMipLevels,
									  0,
									  vk::RemainingArrayLayers}}},

			  m_current_image_index {},
			  m_next_image_timeout {create_info.m_next_image_timeout},
			  m_color_attachment {{},
								  create_info.m_color_attachment_create_info.m_layout,
								  {},
								  {},
								  {},
								  create_info.m_color_attachment_create_info.m_load_operation,
								  create_info.m_color_attachment_create_info.m_store_operation,
								  create_info.m_color_attachment_create_info.m_clear_color},
			  m_depth_stencil_attachment {*m_depth_stencil_view,
										  create_info.m_depth_stencil_attachment_create_info.m_layout,
										  {},
										  {},
										  {},
										  create_info.m_depth_stencil_attachment_create_info.m_load_operation,
										  create_info.m_depth_stencil_attachment_create_info.m_store_operation,
										  create_info.m_depth_stencil_attachment_create_info.m_clear_value}
		{
			// check if a single queue family supports both graphics and present operations
			auto queue_family_indices = std::vector<queue_families::family::index_t> {
				queue_families.graphics().m_index};
			if (not queue_families.supports_combined_graphics_and_present_family())
				queue_family_indices.push_back(queue_families.present().m_index);

			// clamp the prefered image count between the minimum and maximum supported by implementation
			const auto image_count = std::clamp(create_info.m_image_count,
												surface.capabilities().surfaceCapabilities.minImageCount,
												surface.capabilities().surfaceCapabilities.maxImageCount);

			auto swapchain_info = vk::SwapchainCreateInfoKHR {{},
															  **surface,
															  image_count,
															  surface.format().surfaceFormat.format,
															  surface.format().surfaceFormat.colorSpace,
															  surface.extent(),
															  1,
															  create_info.m_image_usage,
															  create_info.m_sharing_mode,
															  queue_family_indices,
															  create_info.m_transform,
															  create_info.m_alpha,
															  surface.present_mode()};

			m_object = {*logical_device, swapchain_info};

			m_views.reserve(m_object.getImages().size());

			auto image_view_info = vk::ImageViewCreateInfo(
				{},
				{},
				vk::ImageViewType::e2D,
				surface.format().surfaceFormat.format,
				{},
				{create_info.m_image_aspect, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers});

			for (auto& image : m_object.getImages())
			{
				image_view_info.image = image;
				m_views.emplace_back(*logical_device, image_view_info);
			}
		}

		auto swapchain::surface() const -> const vulkan::surface&
		{
			return m_surface;
		}

		auto swapchain::views() const -> const std::vector<vk::raii::ImageView>&
		{
			return m_views;
		}

		auto swapchain::depth_stencil_buffer() const -> const image&
		{
			return m_depth_stencil_buffer;
		}

		auto swapchain::depth_stencil_view() const -> const vk::raii::ImageView&
		{
			return m_depth_stencil_view;
		}

		auto swapchain::next_image_info(const vk::raii::CommandBuffer& command_buffer,
										const vk::raii::Semaphore& semaphore)
			-> const std::tuple<vk::Result, image_index_t, vk::RenderingInfo>
		{
			auto [result, image_index] = m_object.acquireNextImage(m_next_image_timeout, *semaphore);

			m_current_image_index = image_index;
			m_color_attachment.imageView = *m_views[m_current_image_index];

			// transition_layout_for_rendering(command_buffer);
			transition_layout<layout_state::rendering>(command_buffer);

			return {result,
					m_current_image_index,
					vk::RenderingInfo {vk::RenderingFlagBits {},
									   {{0, 0}, m_surface.extent()},
									   1,
									   0,
									   m_color_attachment,
									   &m_depth_stencil_attachment,
									   &m_depth_stencil_attachment}};
		}
		auto swapchain::current_image_index() const -> const image_index_t&
		{
			return m_current_image_index;
		}
	}
}
