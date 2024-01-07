module;

module default_meshes;

namespace lh
{
	default_meshes::default_meshes(const vulkan::logical_device& logical_device,
								   const vulkan::memory_allocator& memory_allocator,
								   const std::vector<std::filesystem::path>& model_paths,
								   const create_info& create_info)
		: m_default_meshes {}
	{
		m_default_meshes.reserve(model_paths.size());

		for (const auto& path : model_paths)
		{
			// const auto scene_loader = lh::scene_loader {logical_device, memory_allocator, path};
			m_default_meshes.push_back(
				std::move(lh::scene_loader {logical_device, memory_allocator, path}.meshes().back()));
			// m_default_meshes.back() = std::move(scene_loader.meshes().back());
			// std::move(scene_loader.meshes().begin(), scene_loader.meshes().end(), m_default_meshes.end());
		}
	}
}
