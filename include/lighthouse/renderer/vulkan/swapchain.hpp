#pragma once

#include "lighthouse/renderer/vulkan/raii_wrapper.hpp"
#include "lighthouse/renderer/vulkan/image.hpp"

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

			enum class layout_state
			{
				rendering,
				presentation
			};

			struct create_info
			{
				struct color_attachment_create_info
				{
					vk::ImageLayout m_layout = vk::ImageLayout::eAttachmentOptimal;
					vk::AttachmentLoadOp m_load_operation = vk::AttachmentLoadOp::eClear;
					vk::AttachmentStoreOp m_store_operation = vk::AttachmentStoreOp::eStore;
					vk::ClearColorValue m_clear_color = {1.0f, 1.0f, 1.0f, 1.0f};
				};

				struct depth_stencil_attachment_create_info
				{
					vk::ImageLayout m_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
					vk::AttachmentLoadOp m_load_operation = vk::AttachmentLoadOp::eClear;
					vk::AttachmentStoreOp m_store_operation = vk::AttachmentStoreOp::eStore;
					vk::ClearDepthStencilValue m_clear_value = {1.0f, 1u};
				};

				decltype(vk::SwapchainCreateInfoKHR::minImageCount) m_image_count = {2};
				vk::ImageAspectFlagBits m_image_aspect = vk::ImageAspectFlagBits::eColor;
				vk::ImageUsageFlagBits m_image_usage = vk::ImageUsageFlagBits::eColorAttachment;
				vk::SharingMode m_sharing_mode = vk::SharingMode::eExclusive;
				vk::SurfaceTransformFlagBitsKHR m_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
				vk::CompositeAlphaFlagBitsKHR m_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
				vk::ImageViewType m_image_view_type = vk::ImageViewType::e2D;

				color_attachment_create_info m_color_attachment_create_info = {};
				depth_stencil_attachment_create_info m_depth_stencil_attachment_create_info = {};

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
			auto next_image_info(const vk::raii::CommandBuffer&, const vk::raii::Semaphore&)
				-> const std::tuple<vk::Result, image_index_t, vk::RenderingInfo>;

			template <layout_state state>
			auto transition_layout(const vk::raii::CommandBuffer& command_buffer) const -> void;

		private:
			const vulkan::surface& m_surface;

			std::vector<vk::raii::ImageView> m_views;
			image m_depth_stencil_buffer;

			image_index_t m_current_image_index;
			image_timeout_t m_next_image_timeout;

			vk::RenderingAttachmentInfo m_color_attachment;
			vk::RenderingAttachmentInfo m_depth_stencil_attachment;
		};

		template <swapchain::layout_state state>
		auto swapchain::transition_layout(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			constexpr auto queue_ownership = uint32_t {0};

			constexpr auto undefined_layout = vk::ImageLayout::eUndefined;
			constexpr auto present_layout = vk::ImageLayout::ePresentSrcKHR;

			constexpr auto color_layout = vk::ImageLayout::eColorAttachmentOptimal;
			constexpr auto depth_stencil_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

			constexpr auto pipeline_stage = (state == layout_state::rendering)
												? vk::PipelineStageFlagBits2::eTopOfPipe
												: vk::PipelineStageFlagBits2::eBottomOfPipe;

			constexpr auto color_transition_to_rendering = (state == layout_state::rendering) ? undefined_layout
																							  : color_layout;
			constexpr auto depth_stencil_transition_to_rendering = (state == layout_state::rendering)
																	   ? undefined_layout
																	   : depth_stencil_layout;

			constexpr auto color_transition_to_presentation = (state == layout_state::presentation) ? present_layout
																									: color_layout;
			constexpr auto depth_stencil_transition_to_presentation = (state == layout_state::presentation)
																		  ? present_layout
																		  : depth_stencil_layout;

			const auto barriers = std::array<vk::ImageMemoryBarrier2, 2> {
				vk::ImageMemoryBarrier2 {pipeline_stage,
										 {},
										 vk::PipelineStageFlagBits2::eColorAttachmentOutput,
										 vk::AccessFlagBits2::eColorAttachmentWrite,
										 color_transition_to_rendering,
										 color_transition_to_presentation,
										 queue_ownership,
										 queue_ownership,
										 m_object.getImages()[m_current_image_index],
										 {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}},
				vk::ImageMemoryBarrier2 {
					pipeline_stage,
					{},
					vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
					vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
					depth_stencil_transition_to_rendering,
					depth_stencil_transition_to_presentation,
					queue_ownership,
					queue_ownership,
					**m_depth_stencil_buffer,
					{vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1}}};

			const auto dependency_info = vk::DependencyInfo {{}, {}, {}, barriers};
			command_buffer.pipelineBarrier2(dependency_info);
		}

	}
}
