module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module pipeline;

import data_type;
import physical_device;
import logical_device;
import memory_allocator;
import descriptor_buffer;
import pipeline_layout;
import file_system;
import shader_object;
import buffer;
import spir_v;
import shader_input;
import vertex_input_description;
import descriptor_resource_buffer;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import std;

export namespace lh
{
	namespace vulkan
	{
		class pipeline
		{
		public:
			struct glsl_pipelne_stage_data
			{
				filepath_t m_shader_code;
				bool m_cache_spir_v = true;
				bool m_cache_reflection_data = true;
				bool m_cache_shader_object = true;
			};

			struct create_info
			{
				vk::PipelineBindPoint m_bind_point = vk::PipelineBindPoint::eGraphics;
				bool m_cache_spir_v = true;
				bool m_cache_shader_pipeline = true;
			};

			pipeline(const physical_device&,
					 const logical_device&,
					 const memory_allocator&,
					 const shader_pipeline::pipeline_code_t&,
					 const pipeline_layout&,
					 const descriptor_buffer&,
					 const create_info& = {});

			auto vertex_input_description() const -> const std::optional<vulkan::vertex_input_description>&;
			auto shader_pipeline() const -> const vulkan::shader_pipeline&;
			auto resource_buffer() const -> const vulkan::descriptor_resource_buffer&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			struct shader_binaries
			{
				struct binary
				{
					bool m_exists;
					bool m_up_to_date;
				};

				binary m_spir_v;
				binary m_shader_object;
				binary m_reflection_data;
			};

			auto translate_shader_input_format(const shader_input&) const -> const vk::Format;
			auto generate_vertex_input_description(const std::vector<shader_input>&)
				-> const vulkan::vertex_input_description;
			auto generate_shader_binary_tests(const std::filesystem::path&) -> const shader_binaries;

			const create_info m_create_info;
			const descriptor_buffer& m_descriptor_buffer;
			std::optional<vulkan::vertex_input_description> m_vertex_input_description;
			vulkan::shader_pipeline m_shader_pipeline;
			descriptor_resource_buffer m_resource_descriptor_buffer;
		};
	}
}