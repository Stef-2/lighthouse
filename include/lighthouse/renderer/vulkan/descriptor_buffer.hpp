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
		struct buffer_subdata;

		class descriptor_buffer
		{
		public:
			struct create_info
			{
				vk::PipelineBindPoint m_bind_point;
				vk::MemoryPropertyFlags descriptor_collection_memory_properties =
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
				vk::BufferUsageFlags m_descriptor_buffer_usage =
					vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT |
					vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT |
					vk::BufferUsageFlagBits::ePushDescriptorsDescriptorBufferEXT;
			};

			descriptor_buffer(const physical_device&,
							  const logical_device&,
							  const memory_allocator&,
							  const descriptor_set_layout&,
							  const buffer_subdata&,
							  const create_info& = {});

			auto buffer() -> const mapped_buffer&;
			auto bind(const vk::raii::CommandBuffer&, const vk::raii::PipelineLayout&) const -> void;

		private:
			auto descriptor_size(const physical_device&, const vk::DescriptorType&) -> const std::size_t;
			auto descriptor_buffer_usage(const descriptor_set_layout&) -> const vk::BufferUsageFlags;

			std::unique_ptr<mapped_buffer> m_descriptor_buffer;
			std::vector<vk::DescriptorBufferBindingInfoEXT> m_binding_info;

			vk::PipelineBindPoint m_bind_point;
		};
	}
}
