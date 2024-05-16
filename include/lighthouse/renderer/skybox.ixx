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
import descriptor_buffer;
import pipeline;
import dynamic_rendering_state;
import mesh_registry;
import shader_object;
import texture;
import mesh;
import queue;

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
		{};

		skybox(const vulkan::physical_device&,
			   const vulkan::logical_device&,
			   const vulkan::memory_allocator&,
			   const vulkan::global_descriptor&,
			   const vulkan::descriptor_buffer&,
			   const mesh_registry&,
			   const vulkan::shader_pipeline::pipeline_glsl_code_t&,
			   const skybox_texture_paths_t&,
			   vulkan::queue&,
			   const create_info& = {});

		auto mesh() const -> const lh::mesh&;
		auto pipeline() const -> const vulkan::pipeline&;
		auto texture() const -> const vulkan::texture&;

		auto record_drawing(const vk::raii::CommandBuffer&, vulkan::dynamic_rendering_state&) const -> void;

	private:
		const lh::mesh& m_mesh;
		vulkan::pipeline m_pipeline_resource_generator;
		vulkan::texture m_texture;
	};
}