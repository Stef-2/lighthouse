#include "lighthouse/vulkan/swapchain.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/surface.hpp"
#include "lighthouse/vulkan/queue_families.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"

lh::vulkan::swapchain::swapchain(const vulkan::physical_device& physical_device,
								 const vulkan::logical_device& logical_device,
								 const vulkan::surface& surface,
								 const vulkan::queue_families& queue_families,
								 const vulkan::memory_allocator& memory_allocator,
								 const create_info& create_info)
	: m_views {},
	  m_depth_stencil_buffer {physical_device,
							  logical_device,
							  memory_allocator,
							  surface.extent(),
							  image::create_info {.m_format = vk::Format::eD24UnormS8Uint,
												  .m_image_usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
												  .m_image_layout = vk::ImageLayout::eUndefined,
												  .m_image_tiling = vk::ImageTiling::eOptimal,
												  .m_image_aspect = vk::ImageAspectFlagBits::eDepth |
																	vk::ImageAspectFlagBits::eStencil}},
	  m_surface {surface},
	  m_current_image_index {},
	  m_next_image_timeout {create_info.m_next_image_timeout},
	  m_color_attachment {{},
						  vk::ImageLayout::eAttachmentOptimal,
						  {},
						  {},
						  {},
						  vk::AttachmentLoadOp::eClear,
						  vk::AttachmentStoreOp::eStore,
						  create_info.m_clear_color},
	  m_depth_stencil_attachment {*m_depth_stencil_buffer.view(),
								  vk::ImageLayout::eDepthStencilAttachmentOptimal,
								  {},
								  {},
								  {},
								  vk::AttachmentLoadOp::eClear,
								  vk::AttachmentStoreOp::eStore,
								  create_info.m_clear_depth_stencil}
{

	auto queue_family_indices = {queue_families.graphics().m_index, queue_families.present().m_index};

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

	auto image_view_info = vk::ImageViewCreateInfo({},
												   {},
												   create_info.m_image_view_type,
												   surface.format().surfaceFormat.format,
												   {},
												   {create_info.m_image_aspect, 0, 1, 0, 1});

	for (auto& image : m_object.getImages())
	{
		image_view_info.image = image;
		m_views.emplace_back(*logical_device, image_view_info);
	}
}

auto lh::vulkan::swapchain::surface() const -> const vulkan::surface&
{
	return m_surface;
}

auto lh::vulkan::swapchain::views() const -> const std::vector<vk::raii::ImageView>&
{
	return m_views;
}

auto lh::vulkan::swapchain::depth_stencil_buffer() const -> const image&
{
	return m_depth_stencil_buffer;
}

auto lh::vulkan::swapchain::next_image_info(const vk::raii::CommandBuffer& command_buffer,
											const vk::raii::Semaphore& semaphore)
	-> const std::tuple<vk::Result, image_index_t, vk::RenderingInfo>
{
	auto [result, image_index] = m_object.acquireNextImage(m_next_image_timeout, *semaphore);

	m_current_image_index = image_index;
	m_color_attachment.imageView = *m_views[m_current_image_index];

	transition_layout_for_rendering(command_buffer);

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

auto lh::vulkan::swapchain::transition_layout_for_rendering(const vk::raii::CommandBuffer& command_buffer) const -> void
{
	const auto color_barrier =
		vk::ImageMemoryBarrier {{},
								vk::AccessFlagBits::eColorAttachmentWrite,
								vk::ImageLayout::eUndefined,
								vk::ImageLayout::eColorAttachmentOptimal,
								0,
								0,
								m_object.getImages()[m_current_image_index],
								vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
	command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
								   vk::PipelineStageFlagBits::eColorAttachmentOutput,
								   {},
								   {},
								   {},
								   color_barrier);

	const auto depth_stencil_barrier = vk::ImageMemoryBarrier {
		{},
		vk::AccessFlagBits::eDepthStencilAttachmentWrite,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthStencilAttachmentOptimal,
		0,
		0,
		**m_depth_stencil_buffer,
		vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1}};
	command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
								   vk::PipelineStageFlagBits::eEarlyFragmentTests |
									   vk::PipelineStageFlagBits::eLateFragmentTests,
								   {},
								   {},
								   {},
								   depth_stencil_barrier);
}

auto lh::vulkan::swapchain::transition_layout_for_presentation(const vk::raii::CommandBuffer& command_buffer) const
	-> void
{
	const auto color_barrier =
		vk::ImageMemoryBarrier {{},
								vk::AccessFlagBits::eColorAttachmentWrite,
								vk::ImageLayout::eColorAttachmentOptimal,
								vk::ImageLayout::ePresentSrcKHR,
								0,
								0,
								m_object.getImages()[m_current_image_index],
								vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
	command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe,
								   vk::PipelineStageFlagBits::eColorAttachmentOutput,
								   {},
								   {},
								   {},
								   color_barrier);

	const auto depth_stencil_barrier = vk::ImageMemoryBarrier {
		{},
		vk::AccessFlagBits::eDepthStencilAttachmentWrite,
		vk::ImageLayout::eDepthStencilAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		0,
		0,
		**m_depth_stencil_buffer,
		vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1}};
	command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe,
								   vk::PipelineStageFlagBits::eEarlyFragmentTests |
									   vk::PipelineStageFlagBits::eLateFragmentTests,
								   {},
								   {},
								   {},
								   depth_stencil_barrier);
}
