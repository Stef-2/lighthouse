#include "lighthouse/renderer/vulkan/vertex_buffer.hpp"
#include "lighthouse/renderer/vulkan/physical_device.hpp"
#include "lighthouse/renderer/vulkan/logical_device.hpp"
#include "lighthouse/renderer/vulkan/memory_allocator.hpp"
#include "lighthouse/renderer/vulkan/shader_object.hpp"

lh::vulkan::vertex_buffer::vertex_buffer(const physical_device& physical_device,
										 const logical_device& logical_device,
										 const memory_allocator& memory_allocator,
										 const vertex_input_description& vertex_input,
										 const create_info& create_info)
	: m_vertices {}, m_vertex_input {vertex_input}
{
	// shader_object.
}

auto lh::vulkan::vertex_buffer::vertex_input() const -> const vertex_input_description&
{
	return m_vertex_input;
}

auto lh::vulkan::vertex_buffer::vertices() const -> const mapped_buffer&
{
	return m_vertices;
}

auto lh::vulkan::vertex_buffer::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
{
	command_buffer.setVertexInputEXT(m_vertex_input.m_bindings, m_vertex_input.m_attributes);
	command_buffer.bindVertexBuffers(0, {**m_vertices}, {0});
}
