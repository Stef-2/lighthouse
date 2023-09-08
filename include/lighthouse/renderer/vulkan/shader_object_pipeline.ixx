module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module shader_object_pipeline;

import physical_device;
import logical_device;
import memory_allocator;
import descriptor_set_layout;
import shader_object;
import descriptor_buffer;
import spir_v;
import vertex_input_description;

#if not INTELLISENSE
import vulkan;
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		class shader_object_pipeline
		{
		public:
			struct create_info
			{
				vk::ShaderCreateFlagsEXT m_modifier_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
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
			vulkan::descriptor_set_layout m_descriptor_set_layout;
			//vulkan::descriptor_buffer m_resource_descriptor_buffer;
			vulkan::vertex_input_description m_vertex_input_description;
			vk::raii::PipelineLayout m_pipeline_layout;
			std::vector<shader_object> m_shader_objects;
		};
	}
};
