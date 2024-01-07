module;

#if INTELLISENSE
#include <filesystem>
#include <vector>
#endif

export module default_meshes;

#if not INTELLISENSE
import std.filesystem;
#endif

import logical_device;
import memory_allocator;
import scene_loader;
import mesh;

namespace lh
{
		class default_meshes
		{
		public:
			struct create_info
			{

			};

			default_meshes(const vulkan::logical_device&,
						   const vulkan::memory_allocator&,
						   const std::vector<std::filesystem::path>&,
						   const create_info& = {});
		private:
			std::vector<mesh> m_default_meshes;
		};
}