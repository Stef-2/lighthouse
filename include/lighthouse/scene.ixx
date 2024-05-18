module;

#if INTELLISENSE
#include <vector>
#endif

export module scene;

import mesh;
import buffer;
import memory_allocator;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	class scene
	{
	public:

	private:
		vulkan::buffer m_mesh_buffer;
		std::vector<mesh> m_meshes;
	};
}