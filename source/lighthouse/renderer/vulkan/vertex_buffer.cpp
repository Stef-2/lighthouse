#include "lighthouse/renderer/vulkan/vertex_buffer.hpp"

lh::vulkan::vertex_buffer::vertex_buffer(const physical_device& physical_device,
										 const logical_device& logical_device,
										 const memory_allocator& memory_allocator,
										 const vk::DeviceSize& size,
										 const create_info& create_info)
	: m_vertex_input {create_info.m_vertex_input},
	  m_data {physical_device,
			  logical_device,
			  memory_allocator,
			  size,
			  mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eVertexBuffer,
										  .m_allocation_flags = vma::AllocationCreateFlagBits::eMapped,
										  .m_properties = vk::MemoryPropertyFlagBits::eHostVisible |
														  vk::MemoryPropertyFlagBits::eHostCoherent}}
{}

auto lh::vulkan::vertex_buffer::vertex_input() const -> const vertex_input_description&
{
	return m_vertex_input;
}

auto lh::vulkan::vertex_buffer::data() const -> const mapped_buffer&
{
	return m_data;
}

auto lh::vulkan::vertex_buffer::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
{
	command_buffer.setVertexInputEXT(m_vertex_input.m_bindings, m_vertex_input.m_attributes);
	command_buffer.bindVertexBuffers(0, {**m_data}, {0});
}
