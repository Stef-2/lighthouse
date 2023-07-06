#pragma once

#include "lighthouse/renderer/vulkan/index_format.hpp"

// forward declarations
namespace std
{
	namespace filesystem
	{
		class path;
	}
}
namespace Assimp
{
	class Importer;
}
namespace lh
{
	// forward declarations
	struct vertex;

	class scene_loader
	{
	public:
		struct create_info
		{};

		scene_loader(const std::filesystem::path&, const create_info& = {});

		// auto meshes() const -> const std::vector<

	private:
		std::unique_ptr<Assimp::Importer> m_importer;
	};
}
