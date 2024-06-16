module;

module mesh;

namespace lh
{
	mesh::mesh() : m_node {}, m_vertex_and_index_subdata {}, m_bounding_box {} {}

	mesh::mesh(const vulkan::buffer_subdata<vulkan::buffer>& suballocated_buffer_data,
			   const geometry::aabb& bounding_box,
			   non_owning_ptr<lh::node> node)
		: m_node {node ? std::shared_ptr<lh::node> {node} : std::make_shared<lh::node>()},
		  m_vertex_and_index_subdata {suballocated_buffer_data},
		  m_bounding_box {std::move(bounding_box)}
	{}

	mesh::mesh(mesh&& other) noexcept
		: object_index<mesh> {std::move(other)},
		  m_node {std::move(other.m_node)},
		  m_bounding_box {std::move(other.m_bounding_box)}

	{}

	mesh& mesh::operator=(mesh&& other) noexcept
	{
		m_node = std::move(other.m_node);
		m_bounding_box = std::move(other.m_bounding_box);

		return *this;
	}

	auto mesh::node() const -> const lh::node&
	{
		return *m_node;
	}

	auto mesh::vertex_subdata() const -> const vulkan::buffer_subdata<vulkan::buffer>::subdata&
	{
		return m_vertex_and_index_subdata[0];
	}

	auto mesh::index_subdata() const -> const vulkan::buffer_subdata<vulkan::buffer>::subdata&
	{
		return m_vertex_and_index_subdata[1];
	}

	auto mesh::bounding_box() const -> const geometry::aabb&
	{
		return m_bounding_box;
	}

	auto mesh::index_count() const -> const std::size_t
	{
		return m_vertex_and_index_subdata[1].m_size / sizeof std::uint32_t;
	}

	auto mesh::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
	{
		command_buffer.bindVertexBuffers(0, {**m_vertex_and_index_subdata.m_buffer}, {0});
		command_buffer.bindIndexBuffer(**m_vertex_and_index_subdata.m_buffer,
									   m_vertex_and_index_subdata.m_subdata[1].m_offset,
									   vk::IndexType::eUint32);
	}
}
