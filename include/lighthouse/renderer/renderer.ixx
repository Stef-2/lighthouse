module;
export module renderer;

import window;
import lighthouse_string;
import version;
import scene_loader;
import instance;
import physical_device;
import logical_device;
import surface;
import queue_families;
import memory_allocator;
import command_control;
import queue;
import global_descriptor;
import material;
import swapchain;
import dynamic_rendering_state;
import descriptor_buffer;
import pipeline_resource_generator;
import camera;
import light;
import default_meshes;

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
		vulkan::queue m_graphics_queue;
		vulkan::queue m_transfer_queue;
		vulkan::swapchain m_swapchain;
		vulkan::dynamic_rendering_state m_dynamic_rendering_state;

		vulkan::global_descriptor m_global_descriptor;
		global_light_manager m_global_light_descriptor_buffer;
		vulkan::descriptor_buffer m_global_descriptor_buffer;
		default_meshes m_default_meshes;

		vulkan::pipeline_resource_generator m_resource_generator;
		scene_loader m_scene_loader;
		lh::camera<camera_type::perspective> m_camera;
		material m_material;
		point_light m_point_light;
		point_light m_point_light2;
		spot_light m_spot_light;
		spot_light m_spot_light2;
		directional_light m_dir_light;
		directional_light m_dir_light2;
		ambient_light m_amb_light;
		ambient_light m_amb_light2;
	};
}
