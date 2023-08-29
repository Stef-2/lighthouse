#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <cstdint>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

module texture;

import output;

namespace lh
{
	namespace vulkan
	{
		texture::texture(const logical_device& logical_device,
						 const memory_allocator& memory_allocator,
						 const std::filesystem::path& path,
						 const create_info& create_info)
			: m_image {nullptr}
		{

			auto width = std::int32_t {};
			auto height = std::int32_t {};
			auto num_color_channels = std::int32_t {};
			constexpr auto rgba_texel_size = std::uint8_t {4};

			const auto image_data =
				stbi_load(path.string().c_str(), &width, &height, &num_color_channels, STBI_rgb_alpha);

			if (not image_data)
			{
				output::error() << "failed to load texture: " + path.string();
				return;
			}

			const auto staging_buffer =
				mapped_buffer {logical_device,
							   memory_allocator,
							   static_cast<std::uint32_t>(width) * static_cast<std::uint32_t>(height) * rgba_texel_size,
							   mapped_buffer::create_info {
								   .m_usage = vk::BufferUsageFlagBits::eTransferSrc,
								   .m_allocation_flags = vma::AllocationCreateFlagBits::eMapped |
														 vma::AllocationCreateFlagBits::eHostAccessSequentialWrite}};

			m_image = vulkan::image {logical_device,
									 memory_allocator,
									 vk::Extent2D {static_cast<std::uint32_t>(width),
												   static_cast<std::uint32_t>(height)},
									 create_info.m_image_create_info};

			const auto memory_to_image_copy = vk::MemoryToImageCopyEXT {
				image_data,
				static_cast<std::uint32_t>(width),
				static_cast<std::uint32_t>(height),
				vk::ImageSubresourceLayers {create_info.m_image_create_info.m_image_aspect,
											create_info.m_image_create_info.m_mip_levels - 1,
											0,
											1},
				vk::Offset3D {0, 0, 0},
				vk::Extent3D {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), 1}};

			logical_device->copyMemoryToImageEXT({vk::CopyMemoryToImageInfoEXT {{vk::HostImageCopyFlagBitsEXT::eMemcpy},
																				**m_image,
																				vk::ImageLayout::eReadOnlyOptimal,
																				memory_to_image_copy}});
		}
	}
}
