#pragma once

#include "lighthouse/renderer/vulkan/index_format.hpp"

#include "assimp/postprocess.h"

#if INTELLISENSE
#include "lighthouse/node.ixx"
#else
import node;
#endif

#include <filesystem>

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
			std::int32_t m_importer_postprocess = aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
												  aiProcess_GenSmoothNormals | aiProcess_GenUVCoords |
												  aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality |
												  aiProcess_FlipWindingOrder | aiProcess_LimitBoneWeights |
												  aiProcess_GenBoundingBoxes;
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
