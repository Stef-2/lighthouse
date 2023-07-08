#pragma once

#include "lighthouse/renderer/vulkan/index_format.hpp"

#include "assimp/postprocess.h"

// forward declarations
namespace std
{
	namespace filesystem
	{
		class path;
	}
}
namespace Assimp
{
	class Importer;
}
namespace lh
{
	class mesh;

	namespace vulkan
	{
		class logical_device;
		class memory_allocator;
		struct vertex;
	}

	class scene_loader
	{
	public:
		struct create_info
		{
			std::uint32_t m_importer_postprocess = aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals |
												   aiProcess_GenUVCoords | aiProcess_JoinIdenticalVertices |
												   aiProcess_ImproveCacheLocality | aiProcess_FlipWindingOrder;
		};

		scene_loader(const vulkan::logical_device&,
					 const vulkan::memory_allocator&,
					 const std::filesystem::path&,
					 const create_info& = {});

		auto meshes() const -> const std::vector<mesh>&;

	private:
		Assimp::Importer* m_importer;
		std::vector<mesh> m_meshes;
	};
}
