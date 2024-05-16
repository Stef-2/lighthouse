module;

#if INTELLISENSE
#include <filesystem>
#include <vector>
#endif

export module mesh_registry;

#if not INTELLISENSE
import std.filesystem;
#endif

import logical_device;
import memory_allocator;
import scene_loader;
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

		mesh_registry(const vulkan::logical_device&,
					   const vulkan::memory_allocator&,
					   const create_info& = {});

		auto plane() const -> const lh::mesh&;
		auto cube() const -> const lh::mesh&;
		auto sphere() const -> const lh::mesh&;
		auto cylinder() const -> const lh::mesh&;
		auto cone() const -> const lh::mesh&;

	private:
		std::vector<mesh> m_default_meshes;
	};
}