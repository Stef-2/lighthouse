module;

module scene;

namespace lh
{
	auto scene::generate_mesh_subdata(const scene_reader& scene_reader) const
		-> const vulkan::buffer_subdata<vulkan::buffer>
	{
		const auto& mesh_data = scene_reader.mesh_data();
		const auto subdata_count = mesh_data.size();

		auto mesh_subdata = vulkan::buffer_subdata<vulkan::buffer> {};
		mesh_subdata.m_subdata.reserve(subdata_count);

		for (auto i = std::size_t {}; const auto& mesh : mesh_data)
		{
			const auto vertex_offset = vk::DeviceAddress {std::distance(mesh_data.front().m_vertices, mesh.m_vertices)};
			const auto vertex_size =
				i < mesh_data.size()
					? vk::DeviceSize {std::distance(mesh.m_vertices, mesh_data.front().m_vertices + i)}
					: vk::DeviceSize {std::distance(mesh.m_vertices, scene_reader.vertex_data().end())};

			// const auto vertex_subdata = vulkan::buffer_subdata<vulkan::buffer>::subdata {.m_offset = };

			const auto index_offset = vk::DeviceAddress {std::distance(mesh_data.front().m_indices, mesh.m_indices)};
			const auto index_size =
				i < mesh_data.size() ? vk::DeviceSize {std::distance(mesh.m_indices, mesh_data.front().m_indices + i)}
									 : vk::DeviceSize {std::distance(mesh.m_indices, scene_reader.vertex_data().end())};

			mesh_subdata.m_subdata.emplace_back(std::distance(mesh_data[i].m_vertices, mesh_data.cbegin() + i), );

			i++;
		}

		return mesh_subdata;
	}
}
