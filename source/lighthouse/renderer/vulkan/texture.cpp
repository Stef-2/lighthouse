module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <cstdint>
#endif

#include "vulkan/vma/vk_mem_alloc.hpp"

module texture;

import input;
import output;

namespace lh
{
	namespace vulkan
	{
		texture::texture() : m_image {nullptr}, m_sampler {nullptr}, m_descriptor {} {}

		texture::texture(const physical_device& physical_device,
						 const logical_device& logical_device,
						 const memory_allocator& memory_allocator,
						 const command_control& command_control,
						 const vk::raii::Queue& queue,
						 const std::filesystem::path& path,
						 const create_info& create_info)
			: m_image {nullptr},
			  m_sampler {logical_device, create_info.m_sampler_create_info},
			  m_extent {},
			  m_num_color_channels {},
			  m_descriptor_image_info {},
			  m_descriptor {}
		{
			/*
			const auto image_data = input::read_file<file_type::image>(path);
			m_extent = vk::Extent3D {image_data.m_width, image_data.m_height, 1};

			const auto staging_buffer = mapped_buffer {logical_device,
													   memory_allocator,
													   image_data.m_data_size,
													   mapped_buffer::create_info {
														   .m_usage = vk::BufferUsageFlagBits::eTransferSrc}};

			staging_buffer.map_data(*image_data.m_data, 0, image_data.m_data_size);

			auto image_create_info = create_info.m_image_create_info;
			image_create_info.m_image_create_info.usage |= vk::ImageUsageFlagBits::eTransferDst;
			image_create_info.m_image_create_info.extent = m_extent;
			m_image = vulkan::image {logical_device, memory_allocator, image_create_info};

			const auto& command_buffer = command_control.first_command_buffer();
			command_buffer.begin(command_control.usage_flags());
			m_image.transition_layout(command_buffer);

			const auto buffer_image_copy =
				vk::BufferImageCopy2 {0, 0, 0, default_image_subresource_layers(), {}, m_extent};

			command_buffer.copyBufferToImage2(vk::CopyBufferToImageInfo2 {
				**staging_buffer, **m_image, vk::ImageLayout::eTransferDstOptimal, buffer_image_copy});

			m_image.transition_layout(
				command_buffer,
				image::layout_transition_data {.m_source_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer,
											   .m_destination_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer,
											   .m_source_access_flags = vk::AccessFlagBits2::eTransferWrite,
											   .m_destination_access_flags = vk::AccessFlagBits2::eTransferRead,
											   .m_old_layout = vk::ImageLayout::eTransferDstOptimal,
											   .m_new_layout = vk::ImageLayout::eShaderReadOnlyOptimal});

			command_buffer.end();

			const auto submit_info = vk::SubmitInfo {{}, {}, {*command_buffer}, {}};
			queue.submit(submit_info, *command_control.fence());

			std::ignore = logical_device->waitForFences(*command_control.fence(), true, 1'000'000'000);
			logical_device->resetFences(*command_control.fence());

			command_control.reset();*/

			generate_image_data(
				logical_device, memory_allocator, command_control, queue, create_info.m_image_create_info, {path});

			generate_descriptor_data(physical_device, logical_device);
		}

		auto texture::image() const -> const vulkan::image&
		{
			return m_image;
		}

		auto texture::sampler() const -> const vulkan::sampler&
		{
			return m_sampler;
		}

		auto texture::descriptor_image_info() const -> const vk::DescriptorImageInfo&
		{
			return m_descriptor_image_info;
		}

		auto texture::descriptor() const -> const std::vector<std::byte>&
		{
			return m_descriptor;
		}

		auto texture::generate_image_data(const logical_device& logical_device,
										  const memory_allocator& memory_allocator,
										  const command_control& command_control,
										  const vk::raii::Queue& queue,
										  const image::create_info& create_info,
										  const std::vector<std::filesystem::path>& paths) -> void

		{
			auto image_data = std::vector<lh::input::image_data> {};
			image_data.reserve(paths.size());

			auto buffer_size = vk::DeviceSize {};

			for (const auto& image_path : paths)
			{
				image_data.emplace_back(input::read_file<file_type::image>(image_path));
				buffer_size += image_data.back().m_data_size;
			}

			m_extent = vk::Extent3D {image_data[0].m_width, image_data[0].m_height, 1};

			const auto staging_buffer = mapped_buffer {logical_device,
													   memory_allocator,
													   buffer_size,
													   mapped_buffer::create_info {
														   .m_usage = vk::BufferUsageFlagBits::eTransferSrc}};

			for (auto buffer_offset = vk::DeviceSize {}; const auto& image : image_data)
			{
				staging_buffer.map_data(*image.m_data, buffer_offset, image.m_data_size);
				buffer_offset += image.m_data_size;

				// check if image dimensions differ
				if (m_extent.width != image.m_width or m_extent.height != image.m_height)
				{
					output::warning() << "image array dimensions differ";
					m_extent = vk::Extent3D {image.m_width, image.m_height, 1};
				}
			}

			auto image_create_info = create_info;
			image_create_info.m_image_create_info.usage |= vk::ImageUsageFlagBits::eTransferDst;
			image_create_info.m_image_create_info.extent = m_extent;
			m_image = vulkan::image {logical_device, memory_allocator, image_create_info};

			const auto& command_buffer = command_control.first_command_buffer();
			command_buffer.begin(command_control.usage_flags());
			m_image.transition_layout(command_buffer);

			const auto buffer_image_copy =
				vk::BufferImageCopy2 {0, 0, 0, default_image_subresource_layers(), {}, m_extent};

			command_buffer.copyBufferToImage2(vk::CopyBufferToImageInfo2 {
				**staging_buffer, **m_image, vk::ImageLayout::eTransferDstOptimal, buffer_image_copy});

			m_image.transition_layout(
				command_buffer,
				image::layout_transition_data {.m_source_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer,
											   .m_destination_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer,
											   .m_source_access_flags = vk::AccessFlagBits2::eTransferWrite,
											   .m_destination_access_flags = vk::AccessFlagBits2::eTransferRead,
											   .m_old_layout = vk::ImageLayout::eTransferDstOptimal,
											   .m_new_layout = vk::ImageLayout::eShaderReadOnlyOptimal});

			command_buffer.end();

			const auto submit_info = vk::SubmitInfo {{}, {}, {*command_buffer}, {}};
			queue.submit(submit_info, *command_control.fence());

			std::ignore = logical_device->waitForFences(*command_control.fence(), true, 1'000'000'000);
			logical_device->resetFences(*command_control.fence());

			command_control.reset();
		}

		auto texture::generate_descriptor_data(const lh::vulkan::physical_device& physical_device,
											   const lh::vulkan::logical_device& logical_device) -> void
		{
			m_descriptor_image_info = vk::DescriptorImageInfo {
				**m_sampler, *m_image.view(), m_image.create_information().m_image_create_info.initialLayout};

			m_descriptor.resize(physical_device.properties()
									.m_descriptor_buffer_properties.m_properties.combinedImageSamplerDescriptorSize);

			logical_device->getDescriptorEXT(
				{vk::DescriptorType::eCombinedImageSampler, {&m_descriptor_image_info}},
				physical_device.properties()
					.m_descriptor_buffer_properties.m_properties.combinedImageSamplerDescriptorSize,
				m_descriptor.data());
		}
	}
}
