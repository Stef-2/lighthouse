module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module pipeline;

import data_type;
import lighthouse_utility;
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
			// possible inputs for individual shader stages
			enum class stage_data_type
			{
				// glsl text file, to be compiled into spir_v, then assembled into a shader object
				glsl,
				// precompiled spir_v, to be assembed into a shader object
				spir_v,
				// cached shader_object data
				shader_object
			};

			struct glsl_create_info
			{
				filepath_t m_shader_data_path;
				std::optional<filepath_t> m_reflection_data_path;
				bool m_cache_spir_v = true;
				bool m_cache_reflection_data = true;
				bool m_cache_shader_object = true;
			};

			struct spir_v_create_info
			{
				filepath_t m_shader_data_path;
				std::optional<filepath_t> m_reflection_data_path;
				bool m_cache_reflection_data = true;
				bool m_cache_shader_object = true;
			};

			struct shader_object_create_info
			{
				filepath_t m_shader_data_path;
				filepath_t m_reflection_data_path;
			};

			using shader_stage_t = std::variant<glsl_create_info, spir_v_create_info, shader_object_create_info>;

			struct create_info
			{
				std::vector<shader_stage_t> m_shaders;
				vk::PipelineBindPoint m_bind_point = vk::PipelineBindPoint::eGraphics;
			};

			pipeline(const physical_device&,
					 const logical_device&,
					 const memory_allocator&,
					 const pipeline_layout&,
					 const descriptor_buffer&,
					 const create_info&);

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
				binary m_reflection_data;
				binary m_shader_object;
			};

			auto translate_shader_input_format(const shader_input&) const -> const vk::Format;
			auto generate_vertex_input_description(const std::vector<shader_input>&)
				-> const vulkan::vertex_input_description;
			auto generate_shader_binary_tests(const filepath_t&) const -> const shader_binaries;
			auto deduce_shader_file_type(const filepath_t&) const -> const std::pair<vk::ShaderStageFlagBits, stage_data_type>;

			const create_info m_create_info;
			const descriptor_buffer& m_descriptor_buffer;
			std::optional<vulkan::vertex_input_description> m_vertex_input_description;
			vulkan::shader_pipeline m_shader_pipeline;
			descriptor_resource_buffer m_resource_descriptor_buffer;
		};
	}
}