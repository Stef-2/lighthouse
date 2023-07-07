#include "lighthouse/renderer/vulkan/vertex_buffer.hpp"
#include "lighthouse/renderer/vulkan/physical_device.hpp"
#include "lighthouse/renderer/vulkan/logical_device.hpp"
#include "lighthouse/renderer/vulkan/memory_allocator.hpp"
#include "lighthouse/renderer/vulkan/vertex_input_description.hpp"
#include "lighthouse/renderer/vulkan/buffer.hpp"
#include "lighthouse/renderer/vulkan/vertex_format.hpp"

lh::vulkan::vertex_buffer::vertex_buffer(const logical_device& logical_device,
										 const memory_allocator& memory_allocator,
										 const std::vector<vertex>& vertices,
										 const std::vector<vertex_index_t>& indices,
										 const vulkan::vertex_input_description& vertex_input,
										 const create_info& create_info)
	: m_vertex_input_description {std::make_unique<vulkan::vertex_input_description>(vertex_input)},
	  m_vertex_and_index_buffer {},
	  m_vertex_and_index_suballocations {}
{
	const auto vertex_buffer_size = sizeof(vertex) * vertices.size();
	const auto index_buffer_size = sizeof(vertex_index_t) * indices.size();

	m_vertex_and_index_buffer = std::make_unique<mapped_buffer>(
		logical_device,
		memory_allocator,
		vertex_buffer_size + index_buffer_size,
		mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eVertexBuffer |
											   vk::BufferUsageFlagBits::eIndexBuffer |
											   vk::BufferUsageFlagBits::eShaderDeviceAddress,
									.m_allocation_flags = vma::AllocationCreateFlagBits::eMapped});

	m_vertex_and_index_suballocations = std::make_unique<buffer_subdata>(
		m_vertex_and_index_buffer.get(),
		std::vector<buffer_subdata::subdata> {{0, vertex_buffer_size}, {vertex_buffer_size, index_buffer_size}});
}

auto lh::vulkan::vertex_buffer::vertex_input_description() const -> const vulkan::vertex_input_description&
{
	return *m_vertex_input_description;
}

auto lh::vulkan::vertex_buffer::vertices() const -> const buffer_subdata
{
	return {m_vertex_and_index_suballocations->m_buffer, {m_vertex_and_index_suballocations->m_subdata[0]}};
}

auto lh::vulkan::vertex_buffer::indices() const -> const buffer_subdata
{
	return {m_vertex_and_index_suballocations->m_buffer, {m_vertex_and_index_suballocations->m_subdata[1]}};
}

auto lh::vulkan::vertex_buffer::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
{
	command_buffer.setVertexInputEXT(m_vertex_input_description->m_bindings, m_vertex_input_description->m_attributes);
	command_buffer.bindVertexBuffers(0, {***m_vertex_and_index_buffer}, {0});
}
