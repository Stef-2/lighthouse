module;

#include "assimp/scene.h"
#include "glm/vec4.hpp"
#include "glm/gtc/random.hpp"

#if INTELLISENSE
#include "lighthouse/renderer/scene_loader.ixx"
#else
module scene_loader;
#endif

import output;

namespace lh
{
	lh::scene_loader::scene_loader(const vulkan::logical_device& logical_device,
								   const vulkan::memory_allocator& memory_allocator,
								   const std::filesystem::path& file_path,
								   const create_info& create_info)
		: m_importer {}, m_meshes {}
	{
		m_importer.ApplyPostProcessing(create_info.m_importer_postprocess);
		const auto scene = m_importer.ReadFile(file_path.generic_string(), create_info.m_importer_postprocess);

		if (not scene)
			output::error() << "could not load a scene: " << file_path.string();

		if (scene->HasMeshes())
			for (auto m = std::size_t {}; m < scene->mNumMeshes; ++m)
			{
				const auto& mesh = *scene->mMeshes[m];

				auto vertices = std::vector<vulkan::vertex> {};
				auto indices = std::vector<vulkan::vertex_index_t> {};

				vertices.reserve(static_cast<std::size_t>(mesh.mNumVertices));
				indices.reserve(static_cast<std::size_t>(mesh.mNumFaces) * 3);

				for (auto v = std::size_t {}; v < mesh.mNumVertices; ++v)
				{
					const auto& vertex = mesh.mVertices[v];
					const auto& normals = mesh.mNormals[v];
					const auto& tex_coords = mesh.HasTextureCoords(0) ? mesh.mTextureCoords[0][v] : aiVector3D {};

					/*
					vertices.emplace_back(glm::vec3 {vertex.x, vertex.y, vertex.z},
										  glm::vec3 {normals.x, normals.y, normals.z},
										  glm::vec2 {tex_coords.x, tex_coords.y});*/

					vertices.emplace_back(glm::vec4 {vertex.x, vertex.y, vertex.z, 1},
										  glm::linearRand(glm::vec4(0), glm::vec4(1)));
				}

				for (auto f = std::size_t {}; f < mesh.mNumFaces; ++f)
				{
					const auto& face = mesh.mFaces[f];
					indices.insert(indices.end(), {face.mIndices[0], face.mIndices[1], face.mIndices[2]});
				}

				const auto bounding_box =
					lh::bounding_box {.m_minima {mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z},
									  .m_maxima {mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z}};

				m_meshes.emplace_back(logical_device, memory_allocator, vertices, indices, bounding_box);
			}
	}

	auto lh::scene_loader::meshes() const -> const std::vector<mesh>&
	{
		return m_meshes;
	}
}
