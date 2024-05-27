module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <vector>
#include <filesystem>
#include <cstddef>
#include <cstdint>
#include <variant>
#endif

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

export module scene_reader;

import node;
import data_type;
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
		using vertex_data_t = data_t;

		// per mesh data
		// stores indices into the data_t vector
		// stores bounding boxes
		struct mesh_iterator
		{
			vertex_data_t::iterator m_vertices;
			vertex_data_t::iterator m_indices;
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

		auto mesh_data() const -> const vertex_data_t&;

	private:
		auto generate_mesh_data(const std::vector<std::filesystem::path>&, const create_info& = {}) -> void;

		Assimp::Importer m_importer;

		// vertex and index data packed into a format suitable for device uploading
		vertex_data_t m_vertex_and_index_data;
		std::vector<mesh_iterator> m_mesh_iterators;
	};
}