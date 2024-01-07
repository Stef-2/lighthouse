module;

module vertex_buffer;

namespace lh
{
	namespace vulkan
	{
		vertex_buffer::vertex_buffer() : m_vertex_and_index_buffer {nullptr}, m_vertex_and_index_suballocations {} {}
		vertex_buffer::vertex_buffer(const logical_device& logical_device,
									 const memory_allocator& memory_allocator,
									 const std::vector<vertex>& vertices,
									 const std::vector<vertex_index_t>& indices,
									 const create_info& create_info)
			: m_vertex_and_index_buffer {}, m_vertex_and_index_suballocations {}
		{
			const auto vertex_buffer_size = sizeof(vertex) * vertices.size();
			const auto index_buffer_size = sizeof(vertex_index_t) * indices.size();

			m_vertex_and_index_buffer = {logical_device,
										 memory_allocator,
										 vertex_buffer_size + index_buffer_size,
										 mapped_buffer::create_info {
											 .m_usage = vk::BufferUsageFlagBits::eVertexBuffer |
														vk::BufferUsageFlagBits::eIndexBuffer |
														vk::BufferUsageFlagBits::eShaderDeviceAddress}};

			m_vertex_and_index_suballocations = {&m_vertex_and_index_buffer,
												 std::vector<buffer_subdata::subdata> {{0, vertex_buffer_size},
																					   {vertex_buffer_size,
																						index_buffer_size}}};
		}

		auto vertex_buffer::vertices() const -> const buffer_subdata
		{
			return {m_vertex_and_index_suballocations.m_buffer, {m_vertex_and_index_suballocations.m_subdata[0]}};
		}

		auto vertex_buffer::indices() const -> const buffer_subdata
		{
			return {m_vertex_and_index_suballocations.m_buffer, {m_vertex_and_index_suballocations.m_subdata[1]}};
		}

		auto vertex_buffer::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			command_buffer.bindVertexBuffers(0, {**m_vertex_and_index_buffer}, {0});
			command_buffer.bindIndexBuffer(**m_vertex_and_index_buffer,
										   m_vertex_and_index_suballocations.m_subdata[1].m_offset,
										   vk::IndexType::eUint32);
		}
	}
}
