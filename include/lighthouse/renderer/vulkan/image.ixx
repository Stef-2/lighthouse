module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module image;

import raii_wrapper;
import logical_device;
import memory_allocator;
import surface;

export namespace lh
{
	namespace vulkan
	{
		constexpr auto default_image_subresource_range() -> const vk::ImageSubresourceRange;
		constexpr auto default_image_subresource_layers() -> const vk::ImageSubresourceLayers;

		class image : public raii_wrapper<vk::raii::Image>
		{
		public:
			using raii_wrapper::raii_wrapper;

			struct create_info
			{
				vk::Format m_format = vk::Format::eR8G8B8A8Srgb;

				vk::ImageCreateInfo m_image_create_info = {{},
														   vk::ImageType::e2D,
														   m_format,
														   vk::Extent3D{},
														   1,
														   1,
														   vk::SampleCountFlagBits::e1,
														   vk::ImageTiling::eOptimal,
														   vk::ImageUsageFlagBits::eSampled,
														   vk::SharingMode::eExclusive};

				vk::ImageViewCreateInfo m_image_view_create_info = {
					{}, {}, vk::ImageViewType::e2D, m_format, {}, default_image_subresource_range()};

				vk::MemoryPropertyFlagBits m_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

				vma::AllocationCreateInfo m_allocation_create_info = {{},
																	  vma::MemoryUsage::eAutoPreferDevice,
																	  m_memory_properties,
																	  m_memory_properties};
			};
			
			struct layout_transition_data
			{
				vk::PipelineStageFlags2 m_source_pipeline_stage = vk::PipelineStageFlagBits2::eTopOfPipe;
				vk::PipelineStageFlags2 m_destination_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer;
				vk::AccessFlags2 m_source_access_flags = {};
				vk::AccessFlags2 m_destination_access_flags = vk::AccessFlagBits2::eTransferWrite;
				vk::ImageLayout m_old_layout = vk::ImageLayout::eUndefined;
				vk::ImageLayout m_new_layout = vk::ImageLayout::eTransferDstOptimal;
				std::uint32_t m_source_queue_family = vk::QueueFamilyIgnored;
				std::uint32_t m_destination_queue_family = vk::QueueFamilyIgnored;
				vk::ImageSubresourceRange m_subresource_range = default_image_subresource_range();
			};

			image(nullptr_t);
			image(const vulkan::logical_device&,
				  const vulkan::memory_allocator&,
				  const create_info& = {});

			auto create_information() const -> const image::create_info&;
			auto view() const -> const vk::raii::ImageView&;
			auto transition_layout(const vk::raii::CommandBuffer&, const layout_transition_data& = {}) -> void;

			auto allocation_info() const -> const vma::AllocationInfo&;
			auto allocation_info() -> vma::AllocationInfo&;
			auto allocation() const -> const vma::Allocation&;

		private:
			create_info m_create_info;
			vk::raii::ImageView m_view;

			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;
		};
	}
}