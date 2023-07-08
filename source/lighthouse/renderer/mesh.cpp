#include "lighthouse/renderer/mesh.hpp"
#include "lighthouse/node.hpp"
#include "lighthouse/renderer/vulkan/buffer.hpp"
#include "lighthouse/renderer/vulkan/vertex_buffer.hpp"
#include "lighthouse/renderer/vulkan/logical_device.hpp"
#include "lighthouse/renderer/vulkan/memory_allocator.hpp"
#include "lighthouse/renderer/vulkan/vertex_format.hpp"

lh::mesh::mesh(const vulkan::logical_device& logical_device,
			   const vulkan::memory_allocator& memory_allocator,
			   const std::vector<vulkan::vertex>& vertices,
			   const std::vector<vulkan::vertex_index_t>& indices,
			   non_owning_ptr<lh::node> node)
	: m_node {node ? std::shared_ptr<lh::node> {node} : std::make_shared<lh::node>()},
	  m_vertices {std::move(vertices)},
	  m_indices {std::move(indices)},
	  m_vertex_buffer {std::make_unique<vulkan::vertex_buffer>(logical_device, memory_allocator, m_vertices, m_indices)}
{
	const auto vertex_buffer = m_vertex_buffer->vertices();
	vertex_buffer.m_buffer->map_data(m_vertices.data());
	vertex_buffer.m_buffer->map_data(m_indices.data(), vertex_buffer.m_subdata[1].m_offset);
}

auto lh::mesh::node() const -> const lh::node&
{
	return *m_node;
}

auto lh::mesh::vertices() const -> const std::vector<vulkan::vertex>&
{
	return m_vertices;
}

auto lh::mesh::indices() const -> const std::vector<vulkan::vertex_index_t>&
{
	return m_indices;
}

auto lh::mesh::vertex_buffer() const -> const vulkan::vertex_buffer&
{
	return *m_vertex_buffer;
}
