module;

#if INTELLISENSE
#include <functional>
#endif

module mesh_registry;

namespace lh
{
	mesh_registry::mesh_registry(const vulkan::logical_device& logical_device,
								 const vulkan::memory_allocator& memory_allocator,
								 vulkan::queue& queue,
								 const create_info& create_info)
		: m_scene_reader {{create_info.m_plane_mesh,
						   create_info.m_cube_mesh,
						   create_info.m_sphere_mesh,
						   create_info.m_cylinder_mesh,
						   create_info.m_cone_mesh}},
		  m_mesh_buffer {logical_device,
						 memory_allocator,
						 std::invoke([this] {
							 auto size = vk::DeviceSize {};
							 for (const auto& mesh : m_scene_reader.mesh_data())
								 size += mesh.m_vertices.size() * sizeof vulkan::vertex +
										 mesh.m_indices.size() * sizeof vulkan::vertex_index_t;
							 return size;
						 }),
						 vulkan::buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eVertexBuffer |
																 vk::BufferUsageFlagBits::eIndexBuffer |
																 vk::BufferUsageFlagBits::eTransferDst}},
		  m_meshes {std::invoke([this] {
			  auto meshes = std::vector<mesh> {};
			  meshes.reserve(m_scene_reader.mesh_data().size());

			  auto total_vertex_size_bytes = vk::DeviceSize {};

			  // index data comes after vertex data
			  // record the total byte size of vertices to be used as an offset for indices
			  for (const auto& mesh : m_scene_reader.mesh_data())
				  total_vertex_size_bytes += mesh.m_vertices.size() * sizeof vulkan::vertex;

			  for (auto vertex_offset = vk::DeviceSize {}, index_offset = vk::DeviceSize {};
				   const auto& mesh : m_scene_reader.mesh_data())
			  {
				  const auto vertex_subdata = vulkan::buffer_subdata<vulkan::buffer>::subdata {
					  vertex_offset, mesh.m_vertices.size() * sizeof vulkan::vertex};
				  const auto index_subdata = vulkan::buffer_subdata<vulkan::buffer>::subdata {
					  total_vertex_size_bytes + index_offset, mesh.m_indices.size() * sizeof vulkan::vertex_index_t};

				  meshes.emplace_back(vulkan::buffer_subdata<vulkan::buffer> {&m_mesh_buffer,
																			  {vertex_subdata, index_subdata}},
									  mesh.m_bounding_box);

				  vertex_offset += mesh.m_vertices.size() * sizeof vulkan::vertex;
				  index_offset += mesh.m_indices.size() * sizeof vulkan::vertex_index_t;
			  }

			  return meshes;
		  })}
	{

		m_mesh_buffer.upload_data(queue, *mesh_data.data(), 0, mesh_data.size() * sizeof std::byte);
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
