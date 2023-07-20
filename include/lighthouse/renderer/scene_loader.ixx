module;
#pragma once

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

export module scene_loader;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/index_format.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/memory_allocator.ixx"
#include "lighthouse/renderer/vulkan/vertex_format.ixx"
#include "lighthouse/renderer/mesh.ixx"
#include "lighthouse/node.ixx"
#else
import node;
import logical_device;
import memory_allocator;
import vertex_format;
import mesh;
import std.filesystem;
#endif

export namespace lh
{
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
		Assimp::Importer m_importer;
		std::vector<mesh> m_meshes;
	};
}
