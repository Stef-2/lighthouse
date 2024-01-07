module;

module default_meshes;

namespace lh
{
	default_meshes::default_meshes(const vulkan::logical_device& logical_device,
								   const vulkan::memory_allocator& memory_allocator,
								   const create_info& create_info)
		: m_default_meshes {}
	{
		auto model_paths = std::vector<std::filesystem::path> {create_info.m_plane_mesh,
															   create_info.m_cube_mesh,
															   create_info.m_sphere_mesh,
															   create_info.m_cylinder_mesh,
															   create_info.m_cone_mesh};

		m_default_meshes.reserve(model_paths.size());

		for (const auto& path : model_paths)
		{
			// const auto scene_loader = lh::scene_loader {logical_device, memory_allocator, path};
			m_default_meshes.push_back(
				std::move(lh::scene_loader {logical_device, memory_allocator, path}.meshes().back()));
			// m_default_meshes = lh::scene_loader {logical_device, memory_allocator, path}.yield_meshes();
			//  m_default_meshes.back() = std::move(scene_loader.meshes().back());
			//  std::move(scene_loader.meshes().begin(), scene_loader.meshes().end(), m_default_meshes.end());
		}
	}

	auto default_meshes::plane() const -> const lh::mesh&
	{
		return m_default_meshes[0];
	}

	auto default_meshes::cube() const -> const lh::mesh&
	{
		return m_default_meshes[1];
	}

	auto default_meshes::sphere() const -> const lh::mesh&
	{
		return m_default_meshes[2];
	}

	auto default_meshes::cylinder() const -> const lh::mesh&
	{
		return m_default_meshes[3];
	}

	auto default_meshes::cone() const -> const lh::mesh&
	{
		return m_default_meshes[4];
	}
}
