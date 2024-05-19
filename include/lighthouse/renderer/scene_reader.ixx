module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <vector>
#include <filesystem>
#include <cstdint>
#endif

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

export module scene_reader;

import node;
import vertex_format;
import index_format;
import geometry;
import mesh;

#if not INTELLISENSE
import std.filesystem;
#endif

export namespace lh
{
	class scene_reader
	{
	public:
		struct mesh_vertex_data
		{
			std::vector<vulkan::vertex> m_vertices;
			std::vector<vulkan::vertex_index_t> m_indices;
			geometry::aabb m_bounding_box;
		};

		struct create_info
		{
			std::int32_t m_importer_postprocess = {
				aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GenNormals | aiProcess_CalcTangentSpace |
				aiProcess_GenUVCoords | aiProcess_Triangulate | aiProcess_FlipWindingOrder |
				aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights |
				aiProcess_GenBoundingBoxes};
		};

		scene_reader(const std::vector<std::filesystem::path>&, const create_info& = {});

		auto mesh_data() const -> const std::vector<mesh_vertex_data>&;

	private:
		Assimp::Importer m_importer;

		std::vector<mesh_vertex_data> m_scene_meshes;
	};
}