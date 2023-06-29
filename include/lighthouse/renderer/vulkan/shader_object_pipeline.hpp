#pragma once

#include <vector>

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;
		class memory_allocator;
		class descriptor_set_layout;
		class shader_object;
		class descriptor_buffer;
		class spir_v;
		struct vertex_input_description;

		class shader_object_pipeline
		{
		public:
			struct create_info
			{
				vk::ShaderCreateFlagsEXT m_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
			};

			shader_object_pipeline(const physical_device&,
								   const logical_device&,
								   const memory_allocator&,
								   const std::vector<spir_v>&,
								   const create_info& = {});

			auto shader_objects() const -> const std::vector<shader_object>&;
			auto pipeline_layout() const -> const vk::raii::PipelineLayout&;
			auto descriptor_set_layout() const -> const descriptor_set_layout&;
			auto descriptor_buffer() const -> const descriptor_buffer&;
			auto vertex_input_description() const -> const vertex_input_description&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			std::unique_ptr<vulkan::descriptor_set_layout> m_descriptor_set_layout;
			std::unique_ptr<vulkan::descriptor_buffer> m_descriptor_buffer;
			std::unique_ptr<vulkan::vertex_input_description> m_vertex_input_description;
			vk::raii::PipelineLayout m_pipeline_layout;
			std::vector<shader_object> m_shader_objects;
		};
	}
};
