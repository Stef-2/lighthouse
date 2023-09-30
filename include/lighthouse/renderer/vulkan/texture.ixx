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
			struct create_info
			{
				image::create_info m_image_create_info = {};
				sampler::create_info m_sampler_create_info = {};
			};

			texture(const physical_device&,
					const logical_device&,
					const memory_allocator&,
					const command_control&,
					const vk::raii::Queue&,
					const std::filesystem::path&,
					const create_info& = {});

			auto image() const -> const vulkan::image&;
			auto sampler() const -> const vulkan::sampler&;
			auto descriptor() const -> const std::vector<std::byte>&;

		private:
			vulkan::image m_image;
			vulkan::sampler m_sampler;

			std::vector<std::byte> m_descriptor;
		};

	}
}