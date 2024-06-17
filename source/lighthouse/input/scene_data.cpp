module;

#include "assimp/scene.h"

#if INTELLISENSE
	#include "glm/vec3.hpp"
	#include "glm/vec2.hpp"
	#include "glm/mat4x4.hpp"
	#include "glm/gtc/type_ptr.hpp"
	#include "glm/gtc/random.hpp"
#endif

module scene_data;

import vertex_format;
import index_format;
import output;

#if not INTELLISENSE
import glm;
#endif

namespace
{
	auto assimp_transform_to_native(const aiMatrix4x4& transform)
	{
		return lh::geometry::transformation_t {transform.a1,
											   transform.b1,
											   transform.c1,
											   transform.d1,
											   transform.a2,
											   transform.b2,
											   transform.c2,
											   transform.d2,
											   transform.a3,
											   transform.b3,
											   transform.c3,
											   transform.d3,
											   transform.a4,
											   transform.b4,
											   transform.c4,
											   transform.d4};
	}

	auto assimp_mesh_to_node(const aiNode& node, const std::uint32_t mesh_index)
	{
		for (auto i = std::size_t {}; i < node.mNumMeshes; i++)
			if (node.mMeshes[i] == mesh_index) return &node;

		for (auto i = std::size_t {}; i < node.mNumChildren; i++)
			assimp_mesh_to_node(*node.mChildren[i], mesh_index);

		std::unreachable();
	}
}

namespace lh
{
	scene_data::scene_data(const std::vector<std::filesystem::path>& file_paths, const create_info& create_info)
		: m_importer {}, m_vertex_data {}
	{
		m_importer.ApplyPostProcessing(create_info.m_importer_postprocess);

		generate_mesh_data(file_paths, create_info);
	}

	auto scene_data::vertex_data() const -> const vertex_data_t&
	{
		return m_vertex_data;
	}

	auto scene_data::mesh_data() const -> const std::vector<struct mesh_data>&
	{
		return m_mesh_data;
	}

	auto scene_data::generate_mesh_data(const std::vector<std::filesystem::path>& file_paths,
										const lh::scene_data::create_info& create_info) -> void
	{
		constexpr auto vertex_size = sizeof lh::vulkan::vertex;
		constexpr auto index_size = sizeof lh::vulkan::vertex_index_t;

		for (auto byte_count = std::size_t {}; const auto& file_path : file_paths)
		{
			const auto scene = m_importer.ReadFile(file_path.string(), create_info.m_importer_postprocess);
			if (not scene) lh::output::error() << "could not load a scene: " << file_path.string();

			if (scene->HasMeshes())
				// iterate scene meshes
				for (auto m = std::size_t {}; m < scene->mNumMeshes; m++)
				{
					const auto& mesh = *scene->mMeshes[m];

					// find a scene node whose transformation is associated with this mesh
					const auto mesh_node = assimp_mesh_to_node(*scene->mRootNode, m);
					const auto transformation = assimp_transform_to_native(mesh_node->mTransformation);

					const auto mesh_byte_size = mesh.mNumVertices * vertex_size + mesh.mNumFaces * index_size * 3;
					m_vertex_data.resize(m_vertex_data.capacity() + mesh_byte_size);

					const auto vertex_offset = byte_count;
					const auto vertex_buffer_size = mesh.mNumVertices * vertex_size;
					const auto index_offset = vertex_offset + vertex_buffer_size;
					const auto index_buffer_size = mesh.mNumFaces * index_size * 3;
					const auto mesh_data = decltype(std::declval<struct mesh_data>()) {
						vertex_offset,
						vertex_buffer_size,
						index_offset,
						index_buffer_size,
						transformation,
						{.m_minima {mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z},
						 .m_maxima {mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z}}};

					// iterate mesh vertices
					for (auto v = std::size_t {}; v < mesh.mNumVertices; v++)
					{
						const auto& position = mesh.mVertices[v];
						const auto& normal = mesh.mNormals[v];
						const auto& tangent = mesh.mTangents[v];
						const auto& bitangent = mesh.mBitangents[v];
						const auto& tex_coords = mesh.HasTextureCoords(0) ? mesh.mTextureCoords[0][v] : aiVector3D {};

						const auto vertex = lh::vulkan::vertex {glm::vec3 {position.x, position.y, position.z},
																glm::vec3 {normal.x, normal.y, normal.z},
																glm::vec3 {tangent.x, tangent.y, tangent.z},
																glm::vec3 {bitangent.x, bitangent.y, bitangent.z},
																glm::vec2 {tex_coords.x, tex_coords.y}};

						std::memcpy(&m_vertex_data[byte_count], &vertex, vertex_size);
						const auto test = reinterpret_cast<lh::vulkan::vertex*>(m_vertex_data.data());
						byte_count += vertex_size;
					}

					// iterate mesh indices
					for (auto f = std::size_t {}; f < mesh.mNumFaces; f++)
					{
						const auto& face = mesh.mFaces[f];

						std::memcpy(&m_vertex_data[byte_count], face.mIndices, index_size * 3);
						byte_count += index_size * 3;
					}

					// std::cout << "num indices: " << mesh.mNumFaces * 3;
					m_mesh_data.emplace_back(mesh_data);
				}
		}
	}
}
