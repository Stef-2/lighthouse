module;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/image.ixx"
#else
module image;
#endif

namespace lh
{
	namespace vulkan
	{
		image::image(const vulkan::physical_device& physical_device,
					 const vulkan::logical_device& logical_device,
					 const vulkan::memory_allocator& memory_allocator,
					 const vk::Extent2D& extent,
					 const create_info& create_info)
			: m_format(create_info.m_format), m_view {nullptr}, m_memory {nullptr}, m_sampler {logical_device}
		{
			const auto image_info = vk::ImageCreateInfo {create_info.m_image_create_flags,
														 create_info.m_image_type,
														 create_info.m_format,
														 vk::Extent3D(extent, 1),
														 create_info.m_mip_levels,
														 1,
														 create_info.m_image_sample_count,
														 create_info.m_image_tiling,
														 create_info.m_image_usage | vk::ImageUsageFlagBits::eSampled,
														 create_info.m_image_sharing_mode,
														 {}};

			const auto allocation_create_info = vma::AllocationCreateInfo {{}, vma::MemoryUsage::eAuto};
			auto allocation_info = vma::AllocationInfo {};

			auto [image,
				  allocation] = memory_allocator->createImage(image_info, allocation_create_info, allocation_info);

			m_object = {*logical_device, image};

			const auto view_info = vk::ImageViewCreateInfo {{},
															*m_object,
															create_info.m_view_type,
															create_info.m_format,
															{},
															{create_info.m_image_aspect, 0, 1, 0, 1}};

			m_view = {*logical_device, view_info};
			m_memory = {*logical_device, allocation_info.deviceMemory};
		}

		auto image::format() const -> const vk::Format&
		{
			return m_format;
		}

		auto image::view() const -> const vk::raii::ImageView&
		{
			return m_view;
		}

		auto image::memory() const -> const vk::raii::DeviceMemory&
		{
			return m_memory;
		}
	}
}
