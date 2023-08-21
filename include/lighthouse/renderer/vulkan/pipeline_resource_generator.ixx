module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module pipeline_resource_generator;

import physical_device;
import logical_device;
import memory_allocator;
import descriptor_set_layout;
import descriptor_buffer;
import shader_object;
import buffer;
import spir_v;
import shader_input;
import vertex_input_description;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		class pipeline_resource_generator
		{
		public:
			using pipeline_spir_v_code = std::vector<vulkan::spir_v>;

			struct shader_resources
			{
				std::vector<shader_object> m_shader_objects;
				std::vector<vulkan::descriptor_set_layout> m_descriptor_set_layout;
				vk::raii::PipelineLayout m_pipeline_layout;

				vk::VertexInputBindingDescription2EXT m_bindings;
				std::vector<vk::VertexInputAttributeDescription2EXT> m_attributes;
			};

			struct create_info
			{};

			pipeline_resource_generator(const physical_device&,
										const logical_device&,
										const memory_allocator&,
										const pipeline_spir_v_code,
										const create_info& = {});

			auto vertex_input_description() const -> const vulkan::vertex_input_description&;
			auto descriptor_set_layout() const -> const descriptor_set_layout&;
			auto pipeline_layout() const -> const vk::raii::PipelineLayout&;
			auto shader_objects() const -> const std::vector<shader_object>&;
			auto uniform_buffers() const -> const mapped_buffer&;
			auto uniform_buffer_subdata() const -> const buffer_subdata&;
			auto descriptor_buffer() const -> const descriptor_buffer&;

		private:
			auto translate_shader_input_format(const shader_input&) const -> const vk::Format;
			auto generate_vertex_input_description(const std::vector<shader_input>&)
				-> const vulkan::vertex_input_description;

			vulkan::vertex_input_description m_vertex_input_description;
			vulkan::descriptor_set_layout m_descriptor_set_layout;
			vk::raii::PipelineLayout m_pipeline_layout;
			std::vector<shader_object> m_shader_objects;
			mapped_buffer m_uniform_buffers;
			buffer_subdata m_uniform_buffer_subdata;
			vulkan::descriptor_buffer m_descriptor_buffer;
		};
	}
}
