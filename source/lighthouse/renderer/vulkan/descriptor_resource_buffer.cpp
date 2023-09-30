module;

module descriptor_resource_buffer;

namespace lh
{
	namespace vulkan
	{
		descriptor_resource_buffer::descriptor_resource_buffer(const logical_device& logical_device,
															   const memory_allocator& memory_allocator,
															   const vk::DeviceSize& buffer_size,
															   const create_info& create_info)
			: m_data_buffer {}, m_buffer_subdata {}, m_descriptor {}
		{}

		auto descriptor_resource_buffer::mapped_buffer() const -> const vulkan::mapped_buffer&
		{
			return m_data_buffer;
		}

		auto descriptor_resource_buffer::descriptor() const -> const std::vector<std::byte>&
		{
			return m_descriptor;
		}
	}
}
