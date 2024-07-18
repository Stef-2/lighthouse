module;

export module scene;

import mesh;
import buffer;
import memory_allocator;
import scene_data;

import std;

export namespace lh
{
	class scene
	{
	public:

	private:
		//auto generate_mesh_subdata(const scene_data&) const -> const vulkan::buffer_subdata<vulkan::buffer>;

		vulkan::buffer m_mesh_buffer;
		std::vector<mesh> m_meshes;
	};
}