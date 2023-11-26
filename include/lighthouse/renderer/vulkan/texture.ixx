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
			using texture_array_paths_t = std::vector<std::filesystem::path>;

			struct create_info
			{
				image::create_info m_image_create_info = {};
				sampler::create_info m_sampler_create_info = {};
			};

			texture();
			texture(const physical_device&,
					const logical_device&,
					const memory_allocator&,
					const command_control&,
					const vk::raii::Queue&,
					const std::filesystem::path&,
					const create_info& = {});

			auto image() const -> const vulkan::image&;
			auto sampler() const -> const vulkan::sampler&;
			auto descriptor_image_info() const -> const vk::DescriptorImageInfo&;
			auto descriptor() const -> const std::vector<std::byte>&;

		private:
			auto generate_descriptor_data(const lh::vulkan::physical_device&,
										  const lh::vulkan::logical_device&) -> void;
			vulkan::image m_image;
			vulkan::sampler m_sampler;

			vk::Extent3D m_extent;
			std::uint8_t m_num_color_channels;

			vk::DescriptorImageInfo m_descriptor_image_info;
			std::vector<std::byte> m_descriptor;
		};
	}
}