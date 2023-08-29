module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <cstdint>
#include <filesystem>
#endif

export module texture;

import logical_device;
import memory_allocator;
import buffer;
import image;

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
				image::create_info m_image_create_info {.m_image_usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eHostTransferEXT,
														.m_image_layout = vk::ImageLayout::eReadOnlyOptimal,
														.m_memory_type = vk::MemoryPropertyFlagBits::eHostVisible
														};
			};

			texture(const logical_device&,
					const memory_allocator&,
					const std::filesystem::path&,
					const create_info& = {});

			auto image() const -> const vulkan::image&;

		private:
			vulkan::image m_image;
		};
	}
}