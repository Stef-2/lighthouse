module;

#include "glm/mat4x4.hpp"

export module renderer;

import window;
import node;
import lighthouse_string;
import version;
import file_system;
import scene_loader;
import extension;
import instance;
import physical_device;
import logical_device;
import surface;
import queue_families;
import memory_allocator;
import command_control;
import queue;
import image;
import swapchain;
import spir_v;
import shader_object;
import descriptor_set_layout;
import descriptor_buffer;
import pipeline_resource_generator;
import shader_object_pipeline;
import lh_utility;
import vertex_input_description;
import vertex_buffer;
import vertex_format;
import index_format;
import buffer;
import input;
import mesh;
import camera;
import time;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	class renderer
	{
	public:
		struct create_info
		{
			version m_engine_version;
			version m_vulkan_version;

			bool m_using_validation = true;
		};

		renderer(const window&, const create_info&);

		auto render() -> void;

	private:
		auto info(const create_info&) -> string::string_t;

		create_info m_create_info;

		const lh::window& m_window;

		vulkan::instance m_instance;

		vulkan::physical_device m_physical_device;
		vulkan::surface m_surface;

		glm::mat4 m_fake_camera;

		vulkan::queue_families m_queue_families;

		vulkan::logical_device m_logical_device;
		vulkan::memory_allocator m_memory_allocator;
		vulkan::command_control e1m4;

		vulkan::queue m_queue;
		vulkan::swapchain m_swapchain;

		vulkan::mapped_buffer m_common_descriptor_data;

		vulkan::descriptor_set_layout m_descriptor_set_layout;
		vulkan::descriptor_buffer m_descriptor_buffer;

		vulkan::spir_v m_vertex_spirv;
		vulkan::spir_v m_fragment_spirv;
		vulkan::pipeline_resource_generator m_resource_generator;

		vulkan::vertex_input_description m_vertex_input_description;

		vulkan::shader_object m_vertex_object;
		vulkan::shader_object m_fragment_object;
		vulkan::shader_object_pipeline m_shader_object_pipeline;

		vk::raii::PipelineLayout m_pipeline_layout;

		std::vector<vulkan::vertex> m_col_cube_data;
		std::vector<vulkan::vertex_index_t> m_col_cube_indices;

		vulkan::mapped_buffer m_vertex_buffer;
		vulkan::mapped_buffer m_index_buffer;
		scene_loader m_scene_loader;

		vulkan::vertex_buffer m_actual_vb;
		glm::mat4x4 m_model;
		//node m_camera_node;
		lh::camera<camera_type::perspective> m_camera;
		//lh::camera<camera_type::orthographic> m_camera_2;
	};
}
