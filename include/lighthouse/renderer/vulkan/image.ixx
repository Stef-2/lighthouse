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
		class image : public raii_wrapper<vk::raii::Image>
		{
		public:
			using raii_wrapper::raii_wrapper;

			struct create_info
			{
				vk::Format m_format = vk::Format::eR8G8B8A8Srgb;

				vk::ImageType m_image_type = vk::ImageType::e2D;
				vk::ImageCreateFlags m_image_create_flags = vk::ImageCreateFlags {};
				vk::SampleCountFlagBits m_image_sample_count = vk::SampleCountFlagBits::e1;
				vk::ImageUsageFlags m_image_usage = vk::ImageUsageFlagBits::eSampled;
				vk::SharingMode m_image_sharing_mode = vk::SharingMode::eExclusive;
				vk::ImageLayout m_image_layout = vk::ImageLayout::eAttachmentOptimal;
				vk::ImageTiling m_image_tiling = vk::ImageTiling::eOptimal;
				vk::ImageAspectFlags m_image_aspect = vk::ImageAspectFlagBits::eColor;
				decltype(vk::ImageCreateInfo::mipLevels) m_mip_levels = {1};

				vk::ImageViewType m_view_type = vk::ImageViewType::e2D;

				vk::MemoryPropertyFlagBits m_memory_type = vk::MemoryPropertyFlagBits::eDeviceLocal;
			};

			image(nullptr_t);
			image(const vulkan::logical_device&,
				  const vulkan::memory_allocator&,
				  const vk::Extent2D&,
				  const create_info& = {});

			auto format() const -> const vk::Format&;
			auto view() const -> const vk::raii::ImageView&;

			auto allocation_info() const -> const vma::AllocationInfo&;
			auto allocation_info() -> vma::AllocationInfo&;
			auto allocation() const -> const vma::Allocation&;

		private:
			vk::Format m_format;
			vk::raii::ImageView m_view;

			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;
		};
	}
}
