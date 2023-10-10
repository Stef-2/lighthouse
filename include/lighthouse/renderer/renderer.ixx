module;
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
import global_descriptor;
import image;
import texture;
import material;
import swapchain;
import spir_v;
import shader_object;
import descriptor_buffer;
import pipeline_resource_generator;
import lighthouse_utility;
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
		class implementation_inspector
		{
		public:
			implementation_inspector(const vulkan::instance&,
									 const vulkan::physical_device&,
									 const vulkan::logical_device&,
									 const vulkan::memory_allocator&);
		};

		auto info(const create_info&) -> string::string_t;

		create_info m_create_info;

		const lh::window& m_window;

		vulkan::instance m_instance;
		vulkan::physical_device m_physical_device;
		vulkan::surface m_surface;
		vulkan::queue_families m_queue_families;
		vulkan::logical_device m_logical_device;
		vulkan::memory_allocator m_memory_allocator;
		implementation_inspector m_implementation_inspector;
		vulkan::command_control e1m4;
		vulkan::command_control m_transfer_control;
		vulkan::queue m_queue;
		vulkan::swapchain m_swapchain;

		vulkan::global_descriptor m_global_descriptor;
		vulkan::descriptor_buffer m_global_descriptor_buffer;

		vulkan::spir_v m_vertex_spirv;
		vulkan::spir_v m_fragment_spirv;
		vulkan::pipeline_resource_generator m_resource_generator;
		scene_loader m_scene_loader;
		lh::camera<camera_type::perspective> m_camera;
		material m_material;
	};
}
