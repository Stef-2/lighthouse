module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <cstdint>
#endif

#include "vulkan/vma/vk_mem_alloc.hpp"

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
						 const command_control& command_control,
						 const std::filesystem::path& path,
						 const create_info& create_info)
			: m_image {nullptr}
		{
			const auto command_buffer = command_control.first_command_buffer();
			command_buffer.begin(command_control.usage_flags());

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

			const auto image_data_size = static_cast<std::uint32_t>(width) * static_cast<std::uint32_t>(height) *
										 rgba_texel_size;

			const auto staging_buffer =
				mapped_buffer {logical_device,
							   memory_allocator,
							   image_data_size,
							   mapped_buffer::create_info {
								   .m_usage = vk::BufferUsageFlagBits::eTransferSrc,
								   .m_allocation_flags = vma::AllocationCreateFlagBits::eMapped |
														 vma::AllocationCreateFlagBits::eHostAccessSequentialWrite}};

			staging_buffer.map_data(*image_data, 0, image_data_size);
			stbi_image_free(image_data);

			m_image = vulkan::image {logical_device,
									 memory_allocator,
									 vk::Extent2D {static_cast<std::uint32_t>(width),
												   static_cast<std::uint32_t>(height)},
									 create_info.m_image_create_info};

			m_image.transition_layout(command_buffer);

			const auto buffer_image_copy = vk::BufferImageCopy2 {0,
																 0,
																 0,
																 image::default_subresource_layers(),
																 {},
																 vk::Extent3D {static_cast<std::uint32_t>(width),
																			   static_cast<std::uint32_t>(height),
																			   0}};

			command_buffer.copyBufferToImage2(vk::CopyBufferToImageInfo2 {
				**staging_buffer, **m_image, vk::ImageLayout::eTransferDstOptimal, buffer_image_copy});

			m_image.transition_layout(command_buffer,
									  image::layout_transition_data {
										  .m_source_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer,
										  .m_destination_pipeline_stage = vk::PipelineStageFlagBits2::eFragmentShader,
										  .m_source_access_flags = vk::AccessFlagBits2::eTransferWrite,
										  .m_destination_access_flags = vk::AccessFlagBits2::eShaderRead,
										  .m_old_layout = vk::ImageLayout::eTransferDstOptimal,
										  .m_new_layout = vk::ImageLayout::eShaderReadOnlyOptimal});
		}
	}
}
