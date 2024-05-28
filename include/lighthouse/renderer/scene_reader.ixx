module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <cstdint>
#endif

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

export module scene_reader;

import data_type;
import geometry;

import std;

export namespace lh
{
	class scene_reader
	{
	public:
		using vertex_data_t = data_t;

		// per mesh data
		// stores indices into a data_t vector
		// stores bounding boxes
		struct mesh_data
		{
			std::size_t m_vertex_offset;
			std::size_t m_vertex_buffer_size;
			std::size_t m_index_offset;
			std::size_t m_index_buffer_size;

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

		auto vertex_data() const -> const vertex_data_t&;
		auto mesh_data() const -> const std::vector<struct mesh_data>&;

	private:
		auto generate_mesh_data(const std::vector<std::filesystem::path>&, const create_info& = {}) -> void;

		Assimp::Importer m_importer;

		// vertex and index data packed into a format suitable for device uploading
		vertex_data_t m_vertex_data;
		// indices into the data_t vector
		std::vector<struct mesh_data> m_mesh_data;
	};
}