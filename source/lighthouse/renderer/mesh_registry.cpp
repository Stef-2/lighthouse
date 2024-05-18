module;

module mesh_registry;

namespace lh
{
	mesh_registry::mesh_registry(const vulkan::logical_device& logical_device,
								 const vulkan::memory_allocator& memory_allocator,
								 vulkan::queue& queue,
								 const create_info& create_info)
		: m_meshes {scene_reader {logical_device,
								  memory_allocator,
								  {create_info.m_plane_mesh,
								   create_info.m_cube_mesh,
								   create_info.m_sphere_mesh,
								   create_info.m_cylinder_mesh,
								   create_info.m_cone_mesh}}
						.meshes()},
		  m_mesh_buffer {logical_device,
						 memory_allocator,
						 std::invoke([] {
							 auto size = vk::DeviceSize {};
							 for (const auto& mesh : m_meshes)
								 size += mesh.device_size();
							 return size;
						 }),
						 vulkan::buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eVertexBuffer |
																 vk::BufferUsageFlagBits::eIndexBuffer |
																 vk::BufferUsageFlagBits::eTransferDst}}
	{
		auto model_paths = std::vector<std::filesystem::path> {create_info.m_plane_mesh,
															   create_info.m_cube_mesh,
															   create_info.m_sphere_mesh,
															   create_info.m_cylinder_mesh,
															   create_info.m_cone_mesh};
		/*
		m_meshes.reserve(model_paths.size());*/
		/*
		for (const auto& path : model_paths)
		{
			// const auto scene_loader = lh::scene_loader {logical_device, memory_allocator, path};
			m_meshes.push_back(std::move(lh::scene_loader {logical_device, memory_allocator, path}.meshes().back()));
			// m_default_meshes = lh::scene_loader {logical_device, memory_allocator, path}.yield_meshes();
			//  m_default_meshes.back() = std::move(scene_loader.meshes().back());
			//  std::move(scene_loader.meshes().begin(), scene_loader.meshes().end(), m_default_meshes.end());
		}*/
	}

	auto mesh_registry::plane() const -> const lh::mesh&
	{
		return m_meshes[0];
	}

	auto mesh_registry::cube() const -> const lh::mesh&
	{
		return m_meshes[1];
	}

	auto mesh_registry::sphere() const -> const lh::mesh&
	{
		return m_meshes[2];
	}

	auto mesh_registry::cylinder() const -> const lh::mesh&
	{
		return m_meshes[3];
	}

	auto mesh_registry::cone() const -> const lh::mesh&
	{
		return m_meshes[4];
	}
}
