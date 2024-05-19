module;

#include "assimp/scene.h"

#if INTELLISENSE
	#include "glm/vec3.hpp"
	#include "glm/vec2.hpp"
	#include "glm/gtc/random.hpp"
#endif

module scene_reader;
import output;

#if not INTELLISENSE
import glm;
#endif

namespace
{
	auto generate_mesh_data(const std::vector<std::filesystem::path>& file_paths,
							Assimp::Importer& importer,
							const lh::scene_loader::create_info& create_info)
	{
		for (const auto& file_path : file_paths)
		{
			const auto scene = importer.ReadFile(file_path.string(), create_info.m_importer_postprocess);
			if (not scene) lh::output::error() << "could not load a scene: " << file_path.string();

			if (scene->HasMeshes())
				for (auto m = std::size_t {}; m < scene->mNumMeshes; ++m)
				{
					const auto& mesh = *scene->mMeshes[m];

					vertices.reserve(m_vertices.capacity() + static_cast<std::size_t>(mesh.mNumVertices));
					indices.reserve(m_indices.capacity() + static_cast<std::size_t>(mesh.mNumFaces) * 3);

					for (auto v = std::size_t {}; v < mesh.mNumVertices; ++v)
					{
						const auto& position = mesh.mVertices[v];
						const auto& normal = mesh.mNormals[v];
						const auto& tangent = mesh.mTangents[v];
						const auto& bitangent = mesh.mBitangents[v];
						const auto& tex_coords = mesh.HasTextureCoords(0) ? mesh.mTextureCoords[0][v] : aiVector3D {};

						vertices.emplace_back(glm::vec3 {position.x, position.y, position.z},
											  glm::vec3 {normal.x, normal.y, normal.z},
											  glm::vec3 {tangent.x, tangent.y, tangent.z},
											  glm::vec3 {bitangent.x, bitangent.y, bitangent.z},
											  glm::vec2 {tex_coords.x, tex_coords.y});
					}

					for (auto f = std::size_t {}; f < mesh.mNumFaces; ++f)
					{
						const auto& face = mesh.mFaces[f];
						indices.insert(indices.end(), {face.mIndices[0], face.mIndices[1], face.mIndices[2]});
					}

					const auto bounding_box =
						lh::geometry::aabb {.m_minima {mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z},
											.m_maxima {mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z}};

					m_mesh_vertex_data.emplace_back(vertices, indices, bounding_box);
				}
		}
	}
}

namespace lh
{
	lh::scene_loader::scene_reader(const std::vector<std::filesystem::path>& file_paths, const create_info& create_info)
		: m_importer {}, m_scene_meshes {}
	{
		m_importer.ApplyPostProcessing(create_info.m_importer_postprocess);
		generate_mesh_data(file_paths, m_importer, create_info);
	}

	auto mesh_data() const -> const std::vector<m_mesh_vertex_data>&
	{
		return m_scene_meshes;
	}
}
