#pragma once

#include <vector>

namespace lh
{
	// forward declrations
	namespace vulkan
	{
		class physical_device;
		class logical_device;
		class memory_allocator;
		class descriptor_set_layout;
		class descriptor_buffer;
		class shader_object;
		class mapped_buffer;
		class spir_v;
		struct shader_input;
		struct vertex_input_description;

		class pipeline_resource_generator
		{
		public:
			using pipeline_spir_v_code = std::vector<vulkan::spir_v>;

			struct shader_resources
			{
				std::vector<shader_object> m_shader_objects;
				std::vector<descriptor_set_layout> m_descriptor_set_layout;
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

			auto vertex_input_description() const -> const vulkan::vertex_input_description;
			auto descriptor_set_layouts() const -> const std::vector<vulkan::descriptor_set_layout>;

		private:
			auto shader_input_hash(const shader_input&) const -> const std::size_t;
			auto translate_shader_input_format(const shader_input&) const -> const vk::Format;
			auto generate_vertex_input_description(const std::vector<shader_input>&)
				-> const vulkan::vertex_input_description;

			std::unique_ptr<vulkan::vertex_input_description> m_vertex_input_description;
			std::vector<vulkan::descriptor_set_layout> m_descriptor_set_layouts;
		};
	}
}
