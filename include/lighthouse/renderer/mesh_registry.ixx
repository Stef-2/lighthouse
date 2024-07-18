module;

export module mesh_registry;

import logical_device;
import buffer;
import queue;
import memory_allocator;
import scene_data;
import geometry;
import mesh;
import registry;

import std;

export namespace lh
{
	class mesh_registry : public registry<mesh>
	{
	public:
		enum class default_meshes
		{
			plane,
			cube,
			sphere,
			cylinder,
			cone,
			default_mesh_count
		};

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
		std::array<mesh, std::to_underlying(default_meshes::default_mesh_count)> m_default_meshes;
		std::vector<vulkan::buffer> m_mesh_buffers;
	};
}