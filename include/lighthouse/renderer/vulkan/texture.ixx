module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <cstdint>
#include <filesystem>
#endif

export module texture;

import logical_device;
import memory_allocator;
import command_control;
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
				image::create_info m_image_create_info {.m_image_usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
														.m_image_layout = vk::ImageLayout::eTransferDstOptimal,
														.m_memory_type = vk::MemoryPropertyFlagBits::eDeviceLocal
														};
			};

			texture(const logical_device&,
					const memory_allocator&,
					const command_control&,
					const vk::raii::Queue&,
					const std::filesystem::path&,
					const create_info& = {});

			auto image() const -> const vulkan::image&;

		private:
			vulkan::image m_image;
		};
	}
}