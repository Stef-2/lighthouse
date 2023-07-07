#include "lighthouse/renderer/scene_loader.hpp"
#include "lighthouse/input.hpp"

#include "assimp/importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

lh::scene_loader::scene_loader(const std::filesystem::path& file_path, const create_info& create_info)
	: m_importer {std::make_unique<Assimp::Importer>()}
{
	m_importer->ReadFile(file_path.generic_string(), 0);

	const auto scene = m_importer->GetScene();

	if (scene->HasMeshes())
		for (auto i = std::size_t {}; i < scene->mNumMeshes; ++i)
		{
			const auto& mesh = *scene->mMeshes[i];
		}
}
