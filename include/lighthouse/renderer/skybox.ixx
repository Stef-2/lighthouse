module;

#if INTELLISENSE
#include <array>
#include <filesystem>
#endif

export module skybox;

import mesh;
import shader_object;
import logical_device;
import memory_allocator;

#if not INTELLISENSE
import std.core;
import std.filesystem;
#endif

export namespace lh
{
	class skybox
	{
	public:


		struct create_info
		{

		};

		skybox(const vulkan::logical_device&,
			   const vulkan::memory_allocator&,
			   const std::array<std::filesystem::path, 6>&,
			   const create_info& = {});

	private:
		mesh m_mesh;
		vulkan::shader_object m_shader_object;
	};
}