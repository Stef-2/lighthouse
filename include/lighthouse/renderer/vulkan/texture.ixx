module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <cstdint>
#include <filesystem>
#include <vector>
#endif

export module texture;

import physical_device;
import logical_device;
import memory_allocator;
import command_control;
import buffer;
import image;
import image_view;
import sampler;

#if not INTELLISENSE
import vulkan;
import std.core;
import std.filesystem;
#endif

export namespace lh
{
	namespace vulkan
	{
		class texture
		{
		public:
			using image_paths_t = std::vector<std::filesystem::path>;

			struct create_info
			{
				image::create_info m_image_create_info = {};
				image_view::create_info m_image_view_create_info = {};
				sampler::create_info m_sampler_create_info = {};
			};

			texture();
			texture(const physical_device&,
					const logical_device&,
					const memory_allocator&,
					const command_control&,
					const vk::raii::Queue&,
					const image_paths_t&,
					const create_info& = {});

			auto image() const -> const vulkan::image&;
			auto view() const -> const vulkan::image_view&;
			auto sampler() const -> const vulkan::sampler&;
			auto extent() const -> const vk::Extent3D&;

			auto descriptor_image_info() const -> const vk::DescriptorImageInfo&;
			auto descriptor() const -> const std::vector<std::byte>&;

		protected:
			auto generate_image_data(const logical_device&,
									 const memory_allocator&,
									 const command_control&,
									 const vk::raii::Queue&,
									 const image::create_info&,
									 const std::vector<std::filesystem::path>&) -> void;

			auto generate_descriptor_data(const lh::vulkan::physical_device&,
										  const lh::vulkan::logical_device&) -> void;
			vulkan::image m_image;
			vulkan::image_view m_image_view;
			vulkan::sampler m_sampler;
			
			std::uint8_t m_num_color_channels;

			vk::DescriptorImageInfo m_descriptor_image_info;
			std::vector<std::byte> m_descriptor;
		};
	}
}