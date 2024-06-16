module;

#if INTELLISENSE
	#include "vulkan/vulkan.hpp"

	#include <cstdint>
#endif

module texture;

import input;
import image_data;
import output;
import vulkan_utility;

namespace lh
{
	namespace vulkan
	{
		texture::texture(const physical_device& physical_device,
						 const logical_device& logical_device,
						 const memory_allocator& memory_allocator,
						 queue& queue,
						 const image_paths_t& paths,
						 const descriptor_buffer& descriptor_buffer,
						 const create_info& create_info)
			: m_descriptor_buffer {descriptor_buffer},
			  m_image {nullptr},
			  m_image_view {nullptr},
			  m_sampler {logical_device, create_info.m_sampler_create_info},
			  m_num_color_channels {},
			  m_descriptor_image_info {},
			  m_descriptor {},
			  m_descriptor_index {}
		{
			generate_image_data(logical_device, memory_allocator, queue, create_info.m_image_create_info, paths);
			m_image_view = {logical_device, m_image, create_info.m_image_view_create_info};
			generate_descriptor_data(physical_device, logical_device);
			push_descriptor_data_onto_stack(physical_device);
		}

		texture::~texture()
		{
			// mark our index into descriptor buffer as vacant for reuse
			m_descriptor_buffer.m_vacant_combined_image_sampler_slots.push_back(m_descriptor_index);
		}

		auto texture::image() const -> const vulkan::image&
		{
			return m_image;
		}

		auto texture::view() const -> const vulkan::image_view&
		{
			return m_image_view;
		}

		auto texture::sampler() const -> const vulkan::sampler&
		{
			return m_sampler;
		}

		auto texture::extent() const -> const vk::Extent3D&
		{
			return m_image.create_information().m_image_create_info.extent;
		}

		auto texture::descriptor_image_info() const -> const vk::DescriptorImageInfo&
		{
			return m_descriptor_image_info;
		}

		auto texture::descriptor() const -> const std::vector<std::byte>&
		{
			return m_descriptor;
		}

		auto texture::descriptor_index() const -> const descriptor_index_t&
		{
			return m_descriptor_index;
		}

		auto texture::generate_image_data(const logical_device& logical_device,
										  const memory_allocator& memory_allocator,
										  queue& queue,
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

			auto extent = vk::Extent3D {image_data[0].m_width, image_data[0].m_height, 1};

			const auto staging_buffer = mapped_buffer {
				logical_device,
				memory_allocator,
				buffer_size,
				buffer::create_info {.m_usage = {vk::BufferUsageFlagBits::eTransferSrc},
									 .m_memory_properties = {vk::MemoryPropertyFlagBits::eHostVisible |
															 vk::MemoryPropertyFlagBits::eHostCoherent},
									 .m_allocation_create_info = {vma::AllocationCreateFlagBits::eMapped,
																  vma::MemoryUsage::eAuto,
																  {vk::MemoryPropertyFlagBits::eHostVisible |
																   vk::MemoryPropertyFlagBits::eHostCoherent},
																  {vk::MemoryPropertyFlagBits::eHostVisible |
																   vk::MemoryPropertyFlagBits::eHostCoherent}}}};

			for (auto buffer_offset = vk::DeviceSize {}; const auto& image : image_data)
			{
				staging_buffer.map_data(*image.m_data, buffer_offset, image.m_data_size);
				buffer_offset += image.m_data_size;

				// check if image dimensions differ
				if (extent.width != image.m_width or extent.height != image.m_height)
				{
					output::warning() << "image array dimensions differ";
					extent = vk::Extent3D {image.m_width, image.m_height, 1};
				}
			}

			auto image_create_info = create_info;
			image_create_info.m_image_create_info.usage |= vk::ImageUsageFlagBits::eTransferDst;
			image_create_info.m_image_create_info.extent = extent;
			m_image = vulkan::image {logical_device, memory_allocator, image_create_info};

			const auto& command_buffer = queue.command_control().front();
			command_buffer.begin(queue.command_control().usage_flags());
			m_image.transition_layout(command_buffer);

			const auto buffer_image_copy =
				vk::BufferImageCopy2 {0, 0, 0, image_view::default_image_subresource_layers, {}, extent};

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
			queue.submit_and_wait();
			/*
			std::ignore = logical_device->waitForFences(*command_control.fence(), true, 1'000'000'000);
			logical_device->resetFences(*command_control.fence());*/

			queue.command_control().reset();
		}

		auto texture::generate_descriptor_data(const lh::vulkan::physical_device& physical_device,
											   const lh::vulkan::logical_device& logical_device) -> void
		{
			m_descriptor_image_info = vk::DescriptorImageInfo {
				**m_sampler, **m_image_view, m_image.create_information().m_image_create_info.initialLayout};

			m_descriptor.resize(physical_device.properties()
									.m_descriptor_buffer_properties.m_properties.combinedImageSamplerDescriptorSize);

			logical_device->getDescriptorEXT(
				{vk::DescriptorType::eCombinedImageSampler, {&m_descriptor_image_info}},
				physical_device.properties()
					.m_descriptor_buffer_properties.m_properties.combinedImageSamplerDescriptorSize,
				m_descriptor.data());
		}

		auto texture::push_descriptor_data_onto_stack(const lh::vulkan::physical_device& physical_device) -> void
		{
			const auto& descriptor_buffer_properties = physical_device.properties().m_descriptor_buffer_properties;
			const auto& descriptor_offset = descriptor_buffer_properties.m_combined_image_sampler_offset;
			const auto aligned_offset =
				utility::aligned_size(static_cast<vk::DeviceSize>(descriptor_offset),
									  descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

			const auto combined_image_sampler_stack_empty =
				m_descriptor_buffer.m_vacant_combined_image_sampler_slots.empty();

			// initially assume the index of the next texture on the stack
			auto descriptor_index_to_fill = m_descriptor_buffer.m_texture_count;

			// if the combined image sampler stack has vacant slots, fill one of them instead
			if (not combined_image_sampler_stack_empty)
			{
				descriptor_index_to_fill = m_descriptor_buffer.m_vacant_combined_image_sampler_slots.back();
				m_descriptor_buffer.m_vacant_combined_image_sampler_slots.pop_back();
			}

			const auto memcpy_destination = static_cast<std::byte*>(
												m_descriptor_buffer.m_descriptor_buffer.mapped_data_pointer()) +
											m_descriptor_buffer.m_combined_image_sampler_buffer_offset +
											descriptor_offset * descriptor_index_to_fill;

			std::memcpy(memcpy_destination, m_descriptor.data(), m_descriptor.size());

			// assign ourselves an index into this stack
			m_descriptor_index = descriptor_index_to_fill;

			m_descriptor_buffer.m_texture_count++;
		}
	}
}
