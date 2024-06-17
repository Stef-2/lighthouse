module;

module mesh;

import index_format;

namespace lh
{
	mesh::mesh() : m_node {}, m_vertex_and_index_subdata {}, m_bounding_box {}, m_index_count {} {}

	mesh::mesh(const vulkan::buffer_subdata<vulkan::buffer>& suballocated_buffer_data,
			   const geometry::aabb& bounding_box,
			   non_owning_ptr<lh::node> node)
		: m_node {node ? std::shared_ptr<lh::node> {node} : std::make_shared<lh::node>()},
		  m_vertex_and_index_subdata {suballocated_buffer_data},
		  m_bounding_box {std::move(bounding_box)},
		  m_index_count {m_vertex_and_index_subdata[1].m_size / sizeof vulkan::vertex_index_t}
	{}

	mesh::mesh(mesh&& other) noexcept
		: object_index<mesh> {std::exchange(other, {})},
		  m_node {std::exchange(other.m_node, {})},
		  m_vertex_and_index_subdata {std::exchange(other.m_vertex_and_index_subdata, {})},
		  m_bounding_box {std::exchange(other.m_bounding_box, {})},
		  m_index_count {std::exchange(other.m_index_count, {})}
	{}

	mesh& mesh::operator=(mesh&& other) noexcept
	{
		m_node = std::exchange(other.m_node, {});
		m_vertex_and_index_subdata = std::exchange(other.m_vertex_and_index_subdata, {});
		m_bounding_box = std::exchange(other.m_bounding_box, {});
		m_index_count = std::exchange(other.m_index_count, {});

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
		// std::cout << m_vertex_and_index_subdata[1].m_size / sizeof std::uint32_t;
		return m_index_count;
	}

	auto mesh::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
	{
		command_buffer.bindVertexBuffers(0,
										 {**m_vertex_and_index_subdata.m_buffer},
										 {m_vertex_and_index_subdata.m_subdata[0].m_offset});

		command_buffer.bindIndexBuffer(**m_vertex_and_index_subdata.m_buffer,
									   m_vertex_and_index_subdata.m_subdata[1].m_offset,
									   vk::IndexType::eUint32);
	}
}
