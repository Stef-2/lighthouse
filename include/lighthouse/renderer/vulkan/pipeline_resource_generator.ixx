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
import descriptor_set_layout;
import descriptor_buffer;
import global_descriptor;
import file_system;
import shader_object;
import buffer;
import spir_v;
import shader_input;
import vertex_input_description;

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
			using pipeline_glsl_code = std::vector<std::filesystem::path>;

			struct create_info
			{};

			pipeline_resource_generator(const physical_device&,
										const logical_device&,
										const memory_allocator&,
										const pipeline_glsl_code&,
										const global_descriptor&,
										const create_info& = {});

			auto vertex_input_description() const -> const vulkan::vertex_input_description&;
			auto shader_objects() const -> const std::vector<shader_object>&;
			auto uniform_buffers() const -> const mapped_buffer&;
			auto uniform_buffer_subdata() const -> const buffer_subdata&;

		private:
			auto translate_shader_input_format(const shader_input&) const -> const vk::Format;
			auto generate_vertex_input_description(const std::vector<shader_input>&)
				-> const vulkan::vertex_input_description;
			auto generate_descriptor_set_layouts(const logical_device&, const std::vector<std::pair<vk::ShaderStageFlagBits, shader_input>>&) const
				-> const std::vector<std::vector<vk::DescriptorSetLayout>>;

			std::vector<spir_v> m_spir_v;
			vulkan::vertex_input_description m_vertex_input_description;

			std::vector<shader_object> m_shader_objects;
			mapped_buffer m_uniform_buffers;
			buffer_subdata m_uniform_buffer_subdata;
			//vulkan::descriptor_buffer m_resource_descriptor_buffer;
		};
	}
}