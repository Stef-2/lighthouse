module;

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

export module scene_loader;

import node;
import logical_device;
import memory_allocator;
import vertex_format;
import index_format;
import bounding_volume;
import mesh;
import std.filesystem;

export namespace lh
{
	class scene_loader
	{
	public:
		struct create_info
		{
			std::int32_t m_importer_postprocess = aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
												  /*aiProcess_GenSmoothNormals*/ aiProcess_GenNormals |
												  aiProcess_GenUVCoords | aiProcess_Triangulate |
												  aiProcess_FlipWindingOrder |
												  aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality |
												  aiProcess_LimitBoneWeights | aiProcess_GenBoundingBoxes /*|
												  aiProcess_ConvertToLeftHanded*/;
		};

		scene_loader(const vulkan::logical_device&,
					 const vulkan::memory_allocator&,
					 const std::filesystem::path&,
					 const create_info& = {});

		auto meshes() const -> const std::vector<mesh>&;

	private:
		Assimp::Importer m_importer;
		std::vector<mesh> m_meshes;
	};
}
