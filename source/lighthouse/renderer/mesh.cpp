module;

module mesh;

namespace lh
{
	mesh::mesh() : object_index<mesh> {}, m_node {}, m_vertices {}, m_indices {}, m_vertex_buffer {}, m_bounding_box {}
	{}

	mesh::mesh(const vulkan::logical_device& logical_device,
			   const vulkan::memory_allocator& memory_allocator,
			   const std::vector<vulkan::vertex>& vertices,
			   const std::vector<vulkan::vertex_index_t>& indices,
			   const geometry::aabb& bounding_box,
			   vulkan::queue& queue,
			   non_owning_ptr<lh::node> node)
		: m_node {node ? std::shared_ptr<lh::node> {node} : std::make_shared<lh::node>()},
		  m_vertices {std::move(vertices)},
		  m_indices {std::move(indices)},
		  m_vertex_buffer {logical_device, memory_allocator, m_vertices, m_indices},
		  m_bounding_box {std::move(bounding_box)}
	{
		const auto& vertex_buffer = m_vertex_buffer.vertices();
		const auto& index_buffer = m_vertex_buffer.indices();

		vertex_buffer.m_buffer->map_data(*m_vertices.data(), 0, sizeof vulkan::vertex * m_vertices.size());
		vertex_buffer.m_buffer->map_data(*m_indices.data(),
										 index_buffer.m_subdata[0].m_offset,
										 sizeof vulkan::vertex_index_t * m_indices.size());
	}

	mesh::mesh(mesh&& other) noexcept
		: object_index<mesh> {std::move(other)},
		  m_node {std::move(other.m_node)},
		  m_vertices {std::move(other.m_vertices)},
		  m_indices {std::move(other.m_indices)},
		  m_vertex_buffer {std::move(other.m_vertex_buffer)},
		  m_bounding_box {std::move(other.m_bounding_box)}

	{}
	/*
	mesh::mesh(const mesh&& other) noexcept
		: m_node {std::move(other.m_node)},
		  m_vertices {std::move(other.m_vertices)},
		  m_indices {std::move(other.m_indices)},
		  m_vertex_buffer {std::move(other.m_vertex_buffer)},
		  m_bounding_box {std::move(other.m_bounding_box)}

	{}*/

	mesh& mesh::operator=(mesh&& other) noexcept
	{
		m_node = std::move(other.m_node);
		m_vertices = std::move(other.m_vertices);
		m_indices = std::move(other.m_indices);
		m_vertex_buffer = std::move(other.m_vertex_buffer);
		m_bounding_box = std::move(other.m_bounding_box);

		return *this;
	}

	auto mesh::node() const -> const lh::node&
	{
		return *m_node;
	}

	auto mesh::vertices() const -> const std::vector<vulkan::vertex>&
	{
		return m_vertices;
	}

	auto mesh::indices() const -> const std::vector<vulkan::vertex_index_t>&
	{
		return m_indices;
	}

	auto mesh::vertex_buffer() const -> const vulkan::vertex_buffer&
	{
		return m_vertex_buffer;
	}

	auto mesh::bounding_box() const -> const geometry::aabb&
	{
		return m_bounding_box;
	}

}
