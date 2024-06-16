module;

#if INTELLISENSE
#include <filesystem>
#include <vector>
#include <map>
#endif

export module mesh_registry;

#if not INTELLISENSE
import std.core;
import std.filesystem;
#endif

import logical_device;
import buffer;
import queue;
import memory_allocator;
import scene_data;
import geometry;
import mesh;

export namespace lh
{
	class mesh_registry
	{
	public:
		struct create_info
		{
			std::filesystem::path m_plane_mesh = {};
			std::filesystem::path m_cube_mesh = {};
			std::filesystem::path m_sphere_mesh = {};
			std::filesystem::path m_cylinder_mesh = {};
			std::filesystem::path m_cone_mesh = {};
		};

		mesh_registry(const vulkan::logical_device&, const vulkan::memory_allocator&, vulkan::queue&, const create_info& = {});

		auto plane() const -> const lh::mesh&;
		auto cube() const -> const lh::mesh&;
		auto sphere() const -> const lh::mesh&;
		auto cylinder() const -> const lh::mesh&;
		auto cone() const -> const lh::mesh&;

	private:
		scene_data m_scene_reader;


		vulkan::buffer m_mesh_buffer;
		std::vector<mesh> m_meshes;
	};
}