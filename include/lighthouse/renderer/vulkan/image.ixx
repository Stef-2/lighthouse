module;
#pragma once

export module image;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/renderer/vulkan/phyiscal_device.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/memory_allocator.ixx"
#include "lighthouse/renderer/vulkan/surface.ixx"
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/renderer/vulkan/sampler.ixx"
#else
import raii_wrapper;
import physical_device;
import logical_device;
import memory_allocator;
import surface;
import sampler;
#endif


export namespace lh
{
	namespace vulkan
	{
		class image : public raii_wrapper<vk::raii::Image>
		{
		public:
			struct create_info
			{
				vk::Format m_format = vk::Format::eR8G8B8A8Srgb;

				vk::ImageType m_image_type = vk::ImageType::e2D;
				vk::ImageCreateFlags m_image_create_flags = vk::ImageCreateFlags {};
				vk::SampleCountFlagBits m_image_sample_count = vk::SampleCountFlagBits::e1;
				vk::ImageUsageFlagBits m_image_usage = vk::ImageUsageFlagBits::eSampled;
				vk::SharingMode m_image_sharing_mode = vk::SharingMode::eExclusive;
				vk::ImageLayout m_image_layout = vk::ImageLayout::eAttachmentOptimal;
				vk::ImageTiling m_image_tiling = vk::ImageTiling::eOptimal;
				vk::ImageAspectFlags m_image_aspect = vk::ImageAspectFlagBits::eColor;
				decltype(vk::ImageCreateInfo::mipLevels) m_mip_levels = {1};

				vk::ImageViewType m_view_type = vk::ImageViewType::e2D;

				vk::MemoryPropertyFlagBits m_memory_type = vk::MemoryPropertyFlagBits::eDeviceLocal;
			};

			image(const vulkan::physical_device&,
				  const vulkan::logical_device&,
				  const vulkan::memory_allocator&,
				  const vk::Extent2D&,
				  const create_info& = {});

			auto format() const -> const vk::Format&;
			auto view() const -> const vk::raii::ImageView&;
			auto memory() const -> const vk::raii::DeviceMemory&;

		private:
			vk::Format m_format;
			vk::raii::ImageView m_view;
			vk::raii::DeviceMemory m_memory;
			sampler m_sampler;
		};
	}
}
