module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module descriptor_buffer;

import vulkan_utility;
import physical_device;
import logical_device;
import memory_allocator;
import global_descriptor;
import buffer;
import material;
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
			using binding_slot_t = std::uint16_t;

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

			auto map_uniform_buffer_data(const binding_slot_t& offset, const buffer_subdata&) -> void;
			auto map_material(const material&) -> void;
			auto map_resource_buffer(const descriptor_resource_buffer&) -> void;

			auto resource_buffer() -> const mapped_buffer&;
			auto combined_image_sampler_buffer() -> const mapped_buffer&;
			auto bind(const vk::raii::CommandBuffer&, const vk::raii::PipelineLayout&) const -> void;

		private:
			const physical_device& m_physical_device;
			const logical_device& m_logical_device;

			vk::PipelineBindPoint m_bind_point;
			
			std::vector<vk::DescriptorBufferBindingInfoEXT> m_resource_descriptor_buffer_binding_info;
			std::vector<vk::DescriptorBufferBindingInfoEXT> m_combined_image_sampler_descriptor_buffer_binding_info;

			mapped_buffer m_resource_descriptor_buffer;
			mapped_buffer m_combined_image_sampler_descriptor_buffer;
		};
	}
}