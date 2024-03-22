module;

#include "assimp/scene.h"

#if INTELLISENSE
	#include "glm/vec3.hpp"
	#include "glm/vec2.hpp"
	#include "glm/gtc/random.hpp"
#endif

module scene_loader;
import output;

#if not INTELLISENSE
import glm;
#endif

namespace
{
	auto generate_meshes(const lh::vulkan::logical_device& logical_device,
						 const lh::vulkan::memory_allocator& memory_allocator,
						 const std::filesystem::path& file_path,
						 Assimp::Importer& importer,
						 const lh::scene_loader::create_info& create_info)
	{
		const auto scene = importer.ReadFile(file_path.string(), create_info.m_importer_postprocess);

		if (not scene) lh::output::error() << "could not load a scene: " << file_path.string();

		auto meshes = std::vector<lh::mesh> {};

		if (scene->HasMeshes())
			for (auto m = std::size_t {}; m < scene->mNumMeshes; ++m)
			{
				const auto& mesh = *scene->mMeshes[m];

				auto vertices = std::vector<lh::vulkan::vertex> {};
				auto indices = std::vector<lh::vulkan::vertex_index_t> {};

				vertices.reserve(static_cast<std::size_t>(mesh.mNumVertices));
				indices.reserve(static_cast<std::size_t>(mesh.mNumFaces) * 3);

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
					lh::bounding_box {.m_minima {mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z},
									  .m_maxima {mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z}};

				meshes.emplace_back(logical_device, memory_allocator, vertices, indices, bounding_box);
			}

		return meshes;
	}
}

namespace lh
{
	lh::scene_loader::scene_loader(const vulkan::logical_device& logical_device,
								   const vulkan::memory_allocator& memory_allocator,
								   const std::filesystem::path& file_path,
								   const create_info& create_info)
		: m_importer {}, m_meshes {}
	{
		m_importer.ApplyPostProcessing(create_info.m_importer_postprocess);
		m_meshes = generate_meshes(logical_device, memory_allocator, file_path, m_importer, create_info);
	}

	auto lh::scene_loader::meshes() const -> const std::vector<mesh>&
	{
		return m_meshes;
	}

	auto scene_loader::meshes() -> std::vector<mesh>&
	{
		return m_meshes;
	}
}
