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
import descriptor_set_layout;
import buffer;
import texture;

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

				binding_slot_t m_num_uniform_buffer_bindings = 8;
				binding_slot_t m_num_combined_image_sampler_bindings = 8;
			};

			descriptor_buffer();
			descriptor_buffer(const physical_device&,
							  const logical_device&,
							  const memory_allocator&,
							  const descriptor_set_layout&,
							  const create_info& = {});

			auto map_resource_data(const buffer_subdata&) -> void;
			auto map_uniform_buffer_data(const binding_slot_t& offset, const buffer_subdata&) -> void;
			auto map_texture_data(const std::vector<const texture&>&) -> void;

			auto resource_buffer() -> const mapped_buffer&;
			auto combined_image_sampler_buffer() -> const mapped_buffer&;
			auto bind(const vk::raii::CommandBuffer&, const vk::raii::PipelineLayout&) const -> void;

		private:
			auto descriptor_size(const physical_device&, const vk::DescriptorType&) -> const std::size_t;
			auto descriptor_buffer_usage(const descriptor_set_layout&) -> const vk::BufferUsageFlags;

			std::vector<vk::DescriptorBufferBindingInfoEXT> m_descriptor_buffer_binding_info;

			const physical_device* m_physical_device;
			const logical_device* m_logical_device;

			const descriptor_set_layout* m_descriptor_set_layout;

			vk::PipelineBindPoint m_bind_point;

			static constexpr inline auto s_basic_uniform_buffer_binding =
				std::array<vk::DescriptorSetLayoutBinding, 2> {
					vk::DescriptorSetLayoutBinding {
						0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAll},
					vk::DescriptorSetLayoutBinding {
						1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAll}};
			vk::raii::DescriptorSetLayout m_basic_uniform_buffer;

			static constexpr inline auto s_basic_combined_image_sampler_binding =
				std::array<vk::DescriptorSetLayoutBinding, 2> {
					vk::DescriptorSetLayoutBinding {
						0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eAll},
					vk::DescriptorSetLayoutBinding {
						1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eAll}};
			vk::raii::DescriptorSetLayout m_basic_combined_image_sampler;

			mapped_buffer m_resource_descriptor_buffer;
			mapped_buffer m_combined_image_sampler_descriptor_buffer;
		};
	}
}