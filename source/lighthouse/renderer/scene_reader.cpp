module;

#include "assimp/scene.h"

#if INTELLISENSE
	#include "glm/vec3.hpp"
	#include "glm/vec2.hpp"
	#include "glm/gtc/random.hpp"
#endif

module scene_reader;

import vertex_format;
import index_format;
import output;

#if not INTELLISENSE
import glm;
#endif

namespace lh
{
	scene_reader::scene_reader(const std::vector<std::filesystem::path>& file_paths, const create_info& create_info)
		: m_importer {}, m_vertex_data {}
	{
		m_importer.ApplyPostProcessing(create_info.m_importer_postprocess);

		generate_mesh_data(file_paths, create_info);
	}

	auto scene_reader::vertex_data() const -> const vertex_data_t&
	{
		return m_vertex_data;
	}

	auto scene_reader::mesh_data() const -> const std::vector<struct mesh_data>&
	{
		return m_mesh_data;
	}

	auto scene_reader::generate_mesh_data(const std::vector<std::filesystem::path>& file_paths,
										  const lh::scene_reader::create_info& create_info) -> void
	{
		constexpr auto vertex_size = sizeof lh::vulkan::vertex;
		constexpr auto index_size = sizeof lh::vulkan::vertex_index_t;

		for (const auto& file_path : file_paths)
		{
			const auto scene = m_importer.ReadFile(file_path.string(), create_info.m_importer_postprocess);
			if (not scene) lh::output::error() << "could not load a scene: " << file_path.string();

			auto mesh_data_iterator = lh::scene_reader::vertex_data_t::iterator {m_vertex_data.begin()};

			if (scene->HasMeshes())
				// iterate scene meshes
				for (auto m = std::size_t {}; m < scene->mNumMeshes; m++)
				{
					const auto& mesh = *scene->mMeshes[m];

					const auto mesh_size_bytes = mesh.mNumVertices * vertex_size + mesh.mNumFaces * index_size * 3;
					auto mesh_data = decltype(std::declval<struct mesh_data>()) {};
					m_vertex_data.reserve(m_vertex_data.capacity() + mesh_size_bytes);

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

						std::memcpy(&(*mesh_data_iterator), &vertex, vertex_size);
						mesh_data.m_vertices = mesh_data_iterator;
						mesh_data_iterator += vertex_size;
					}

					std::memcpy(&(*mesh_data_iterator),
								mesh.mFaces,
								sizeof vulkan::vertex_index_t * mesh.mNumFaces * 3);
					mesh_data.m_indices = mesh_data_iterator;
					mesh_data_iterator += index_size * mesh.mNumFaces * 3;

					mesh_data.m_bounding_box =
						lh::geometry::aabb {.m_minima {mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z},
											.m_maxima {mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z}};

					m_mesh_data.emplace_back(mesh_data);
				}
		}
	}
}
