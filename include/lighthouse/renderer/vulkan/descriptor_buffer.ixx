module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module descriptor_buffer;

import physical_device;
import logical_device;
import memory_allocator;
import global_descriptor;
import buffer;
import descriptor_resource_buffer;

#if not INTELLISENSE
import std.core;
import vulkan;
#endif

export namespace lh
{
	namespace vulkan
	{
		class descriptor_buffer
		{
		public:
			friend class texture;

			struct create_info
			{
				vk::PipelineBindPoint m_bind_point = vk::PipelineBindPoint::eGraphics;
				vk::MemoryPropertyFlags m_descriptor_buffer_memory_properties = {
					vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};
			};
			
			descriptor_buffer(const physical_device&,
							  const logical_device&,
							  const memory_allocator&,
							  const global_descriptor&,
							  const create_info& = {});

			//auto map_texture(const texture&) -> void;
			//auto map_material(const material&) -> void;
			auto map_resource_buffer(const descriptor_resource_buffer&) -> void;

			auto bind(const vk::raii::CommandBuffer&, const vk::raii::PipelineLayout&) const -> void;

		private:
			const physical_device& m_physical_device;
			const logical_device& m_logical_device;

			vk::PipelineBindPoint m_bind_point;
			
			std::vector<vk::DescriptorBufferBindingInfoEXT> m_uniform_descriptor_buffer_binding_info;
			std::vector<vk::DescriptorBufferBindingInfoEXT> m_storage_descriptor_buffer_binding_info;
			std::vector<vk::DescriptorBufferBindingInfoEXT> m_combined_image_sampler_descriptor_buffer_binding_info;
			std::vector<global_descriptor::descriptor_type_size_t> m_vacant_combined_image_sampler_slots;

			mapped_buffer m_uniform_descriptor_buffer;
			mapped_buffer m_storage_descriptor_buffer;
			mapped_buffer m_combined_image_sampler_descriptor_buffer;
		};
	}
}