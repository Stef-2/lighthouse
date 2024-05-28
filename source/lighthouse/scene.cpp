module;

module scene;

namespace lh
{
	auto scene::generate_mesh_subdata(const scene_reader& scene_reader) const
		-> const vulkan::buffer_subdata<vulkan::buffer>
	{
		const auto subdata_count = scene_reader.mesh_data().size();

		auto mesh_subdata = vulkan::buffer_subdata<vulkan::buffer> {};
		mesh_subdata.m_subdata.reserve(subdata_count);

		for (const auto& mesh_data : scene_reader.mesh_data())
		{
			const auto offset = mesh_data.m_vertices == scene_reader.mesh_data().front().m_vertices
									? vk::DeviceSize {}
									: vk::DeviceSize {mesh_data.};
		}

		return mesh_subdata;
	}
}
