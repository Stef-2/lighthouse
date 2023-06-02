#pragma once

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;
		class memory_allocator;
		class descriptor_set_layout;
		class mapped_buffer;

		class descriptor_collection
		{
		public:
			struct create_info
			{};

			descriptor_collection(const physical_device&,
								  const logical_device&,
								  const memory_allocator&,
								  const descriptor_set_layout&,
								  const create_info& = {});

			auto descriptor_buffers() -> std::vector<mapped_buffer>&;
			auto data_buffers() -> std::vector<mapped_buffer>&;
			auto bind(const vk::raii::CommandBuffer&, const vk::raii::PipelineLayout&) const -> void;

		private:
			auto descriptor_size(const physical_device&, const vk::DescriptorType&) -> const std::size_t;

			vk::DescriptorBufferBindingInfoEXT m_binding_info;

			std::vector<mapped_buffer> m_descriptor_buffers;
			std::vector<mapped_buffer> m_data_buffers;
		};
	}
}
