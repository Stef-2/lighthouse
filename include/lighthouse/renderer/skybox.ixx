module;

#if INTELLISENSE
#include <array>
#include <filesystem>
#endif

export module skybox;

import physical_device;
import logical_device;
import memory_allocator;
import global_descriptor;
import pipeline_resource_generator;
import default_meshes;
import shader_object;
import texture;
import mesh;

#if not INTELLISENSE
import std.core;
import std.filesystem;
#endif

export namespace lh
{
	class skybox
	{
	public:
		using skybox_texture_paths_t = std::array<std::filesystem::path, 6>;

		struct create_info
		{

		};

		skybox(const vulkan::physical_device&,
			const vulkan::logical_device&,
			   const vulkan::memory_allocator&,
				const default_meshes&,
			   const skybox_texture_paths_t&,
			   const create_info& = {});

	private:
		const mesh& m_mesh;
		vulkan::pipeline_resource_generator m_pipeline_resource_generator;
		vulkan::texture m_texture;
	};
}