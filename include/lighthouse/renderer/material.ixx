module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <filesystem>
#include <vector>
#endif

export module material;

#if not INTELLISENSE
import std.core;
import std.filesystem;
import vulkan;
#endif

import physical_device;
import logical_device;
import memory_allocator;
import command_control;
import queue;
import texture;

export namespace lh
{
	class material
	{
	public:
		struct create_info
		{

		};

		material(const vulkan::physical_device&,
				 const vulkan::logical_device&,
				 const vulkan::memory_allocator&,
				 const vulkan::command_control&,
				 const vk::raii::Queue&,
				 const std::vector<std::filesystem::path>&,
				 const create_info& = {});

		auto textures() const -> const std::vector<vulkan::texture>&;
	private:
		std::vector<vulkan::texture> m_textures;
	};
}