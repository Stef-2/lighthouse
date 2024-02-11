module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#include <filesystem>
#include <utility>
#endif

export module pipeline_resource_generator;

import physical_device;
import logical_device;
import memory_allocator;
import descriptor_buffer;
import global_descriptor;
import file_system;
import shader_object;
import buffer;
import spir_v;
import shader_input;
import vertex_input_description;
import descriptor_resource_buffer;

#if not INTELLISENSE
import std.core;
import std.filesystem;
#endif

export namespace lh
{
	namespace vulkan
	{
		class pipeline_resource_generator
		{
		public:
			struct create_info
			{};

			pipeline_resource_generator(const physical_device&,
										const logical_device&,
										const memory_allocator&,
										const shader_pipeline::pipeline_glsl_code_t&,
										const global_descriptor&,
										const create_info& = {});

			auto vertex_input_description() const -> const vulkan::vertex_input_description&;
			auto has_vertex_input() const -> const bool;
			auto shader_pipeline() const -> const vulkan::shader_pipeline&;
			auto descriptor_buffer() const -> const vulkan::descriptor_resource_buffer&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			auto translate_shader_input_format(const shader_input&) const -> const vk::Format;
			auto generate_vertex_input_description(const std::vector<shader_input>&)
				-> const vulkan::vertex_input_description;

			vulkan::vertex_input_description m_vertex_input_description;
			vulkan::shader_pipeline m_shader_pipeline;
			descriptor_resource_buffer m_resource_descriptor_buffer;
		};
	}
}