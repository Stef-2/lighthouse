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
		m_mesh_buffer.upload_data(queue, );
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
