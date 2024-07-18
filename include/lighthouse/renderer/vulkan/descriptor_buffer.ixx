module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module descriptor_buffer;

import physical_device;
import logical_device;
import memory_allocator;
import global_descriptor;
import buffer;
import descriptor_resource_buffer;

#if not INTELLISENSE
import vulkan_hpp;
#endif
import std;

export namespace lh
{
	namespace vulkan
	{
		class descriptor_buffer
		{
		public:
			friend class texture;
			friend class pipeline;

			struct create_info
			{
				vk::MemoryPropertyFlags m_descriptor_buffer_memory_properties = {
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};
			};
			
			descriptor_buffer(const physical_device&,
							  const logical_device&,
							  const memory_allocator&,
							  const global_descriptor&,
							  const create_info& = {});


			auto bind(const vk::raii::CommandBuffer&) const -> void;
			auto flush_resource_descriptors() -> void;

		private:
			auto register_resource_buffer(const descriptor_resource_buffer&) const -> void;

			auto map_resource_buffer_offsets(const vk::raii::CommandBuffer&,
											 const descriptor_resource_buffer&,
											 const vk::PipelineBindPoint&) const -> void;

			using descriptor_offsets_t = vk::DeviceSize;

			struct resource_buffer_offsets
			{
				descriptor_offsets_t m_uniform_descriptor_offset;
				descriptor_offsets_t m_storage_descriptor_offset;
			};

			const physical_device& m_physical_device;
			const logical_device& m_logical_device;
			const global_descriptor& m_global_descriptor;

			// resource management
			mutable descriptor_offsets_t m_accumulated_uniform_descriptor_offset;
			mutable descriptor_offsets_t m_accumulated_storage_descriptor_offset;
			mutable std::map<const descriptor_resource_buffer*, resource_buffer_offsets> m_resource_buffer_offsets;
			
			// texture management
			mutable global_descriptor::descriptor_type_size_t m_texture_count;
			mutable std::vector<global_descriptor::descriptor_type_size_t> m_vacant_combined_image_sampler_slots;

			// buffer management
			const vk::DeviceSize m_uniform_buffer_offset;
			const vk::DeviceSize m_storage_buffer_offset;
			const vk::DeviceSize m_combined_image_sampler_buffer_offset;
			const mapped_buffer m_descriptor_buffer;
			const std::array<vk::DescriptorBufferBindingInfoEXT, 3> m_bindings;
		};
	}
}