module;

#if INTELLISENSE
	#include <functional>
#endif

module mesh_registry;

import scene_data;
import output;
import vertex_format;

namespace lh
{
	mesh_registry::mesh_registry(const vulkan::logical_device& logical_device,
								 const vulkan::memory_allocator& memory_allocator,
								 vulkan::transfer_queue& transfer_queue,
								 const create_info& create_info)
		: m_default_meshes {}, m_mesh_buffers {}
	{
		const auto paths = std::vector<std::filesystem::path> {create_info.m_plane_mesh,
															   create_info.m_cube_mesh,
															   create_info.m_sphere_mesh,
															   create_info.m_cylinder_mesh,
															   create_info.m_cone_mesh};

		const auto mesh_data = scene_data(paths);

		if (mesh_data.mesh_data().size() != std::to_underlying(default_meshes::default_mesh_count))
			output::warning() << "could not import all default meshes";

		m_mesh_buffers.emplace_back(logical_device,
									memory_allocator,
									mesh_data.vertex_data().size(),
									vulkan::buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eVertexBuffer |
																			vk::BufferUsageFlagBits::eIndexBuffer |
																			vk::BufferUsageFlagBits::eTransferDst});
		/*
		m_mesh_buffers.back().upload_data(transfer_queue,
										  *mesh_data.vertex_data().data(),
										  0,
										  mesh_data.vertex_data().size());*/

		transfer_queue.upload_data_and_wait(
			m_mesh_buffers.back(),
			vulkan::transfer_queue::data_upload_info<scene_data::vertex_data_t::value_type> {
				*mesh_data.vertex_data().data(), 0, mesh_data.vertex_data().size()});

		for (auto i = std::size_t {}; i < std::to_underlying(default_meshes::default_mesh_count); i++)
		{
			const auto& data = mesh_data.mesh_data()[i];

			const auto buffer_subdata = vulkan::buffer_subdata<vulkan::buffer> {
				&m_mesh_buffers.back(),
				{{data.m_vertex_offset, data.m_vertex_buffer_size}, {data.m_index_offset, data.m_index_buffer_size}}};

			m_default_meshes[i] = {buffer_subdata, data.m_bounding_box};
		}
	}

	auto mesh_registry::plane() const -> const lh::mesh&
	{
		return m_default_meshes[0];
	}

	auto mesh_registry::cube() const -> const lh::mesh&
	{
		return m_default_meshes[1];
	}

	auto mesh_registry::sphere() const -> const lh::mesh&
	{
		return m_default_meshes[2];
	}

	auto mesh_registry::cylinder() const -> const lh::mesh&
	{
		return m_default_meshes[3];
	}

	auto mesh_registry::cone() const -> const lh::mesh&
	{
		return m_default_meshes[4];
	}
}
