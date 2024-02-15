module;

#include "imgui/imgui.h"

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

module renderer;

import output;
import file_system;
import time;
import glm;

// #pragma optimize("", off)
namespace lh
{
	renderer::renderer(const window& window, const create_info& create_info)
		: m_create_info {create_info},
		  m_window {window},
		  m_instance(window,
					 vulkan::instance::create_info {.m_engine_version = create_info.m_engine_version,
													.m_vulkan_version = create_info.m_vulkan_version}),
		  m_physical_device {m_instance},
		  m_surface {window, m_instance, m_physical_device},
		  m_queue_families {m_physical_device, m_surface},
		  m_logical_device {m_physical_device,
							m_queue_families,
							{m_physical_device.extensions().required_extensions()}},
		  m_memory_allocator {m_instance, m_physical_device, m_logical_device},
		  m_implementation_inspector(m_instance, m_physical_device, m_logical_device, m_memory_allocator),
		  m_graphics_queue {m_logical_device, {m_queue_families.graphics()}},
		  m_transfer_queue {m_logical_device, {m_queue_families.transfer()}},
		  m_swapchain {m_physical_device, m_logical_device, m_surface, m_queue_families, m_memory_allocator},
		  m_dynamic_rendering_state {vulkan::dynamic_rendering_state::create_info {
			  .m_viewport = vk::Viewport(0.0f,
										 static_cast<float>(m_surface.extent().height),
										 static_cast<float>(m_surface.extent().width),
										 -static_cast<float>(m_surface.extent().height),
										 0.0f,
										 1.0f),
			  .m_scissor = vk::Rect2D(vk::Offset2D(0, 0), m_surface.extent())}},
		  m_imgui {window,
				   m_instance,
				   m_physical_device,
				   m_logical_device,
				   m_queue_families.graphics(),
				   m_graphics_queue,
				   m_swapchain},
		  m_global_descriptor {m_physical_device, m_logical_device},
		  m_global_light_manager {m_physical_device, m_logical_device, m_memory_allocator},
		  m_global_descriptor_buffer {m_physical_device, m_logical_device, m_memory_allocator, m_global_descriptor},
		  m_default_meshes {m_logical_device,
							m_memory_allocator,
							{file_system::data_path() /= "models/plane.obj",
							 file_system::data_path() /= "models/cube.obj",
							 file_system::data_path() /= "models/sphere.obj",
							 file_system::data_path() /= "models/cylinder.obj",
							 file_system::data_path() /= "models/cone.obj"}},
		  m_test_pipeline {m_physical_device,
						   m_logical_device,
						   m_memory_allocator,
						   {file_system::data_path() /= "shaders/basic.vert",
							file_system::data_path() /= "shaders/basic.frag"},
						   m_global_descriptor,
						   m_global_descriptor_buffer},
		  m_scene_loader {m_logical_device, m_memory_allocator, file_system::data_path() /= "models/cube.obj"},
		  m_camera {std::make_shared<lh::node>(), camera<camera_type::perspective>::create_info {}},
		  m_material {m_physical_device,
					  m_logical_device,
					  m_memory_allocator,
					  m_transfer_queue,
					  {file_system::data_path() /= "textures/grooved_bricks/basecolor.png",
					   file_system::data_path() /= "textures/grooved_bricks/normal.png",
					   file_system::data_path() /= "textures/grooved_bricks/ambientocclusion.png"},
					  m_global_descriptor_buffer},
		  m_point_light {{1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {0.0f, 0.0f, 0.0f}},
		  m_point_light2 {{0.0f, 1.0f, 0.0f, 1.0f}, 1.0f, {0.0f, 1.0f, 0.0f}},
		  m_spot_light {{0.5f, 0.5f, 0.0f, 1.0f}, 1.0f, {0.0f, 0.0f, 1.0f}},
		  m_spot_light2 {{0.0f, 0.5f, 0.5f, 1.0f}, 1.0f, {0.0f, 0.0f, 1.0f}},
		  m_dir_light {{1.0f, 0.5f, 0.5f, 1.0f}, 1.0f, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		  m_dir_light2 {{0.5f, 1.0f, 0.5f, 1.0f}, 1.0f, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		  m_amb_light {{1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {1.0f, 0.0f, 0.0f}},
		  m_amb_light2 {{0.0f, 1.0f, 0.0f, 1.0f}, 0.5f, {0.0f, 0.0f, 0.0f}},
		  m_skybox {m_physical_device,
					m_logical_device,
					m_memory_allocator,
					m_global_descriptor,
					m_global_descriptor_buffer,
					m_default_meshes,
					{file_system::data_path() /= "shaders/skybox.vert",
					 file_system::data_path() /= "shaders/skybox.frag"},
					{
						file_system::data_path() /= "textures/skybox/+x.png",
						file_system::data_path() /= "textures/skybox/-x.png",
						file_system::data_path() /= "textures/skybox/+y.png",
						file_system::data_path() /= "textures/skybox/-y.png",
						file_system::data_path() /= "textures/skybox/+z.png",
						file_system::data_path() /= "textures/skybox/-z.png",
					},
					m_transfer_queue}
	{
		// m_global_descriptor_buffer.map_resource_buffer(m_test_pipeline.resource_buffer());

		if (m_create_info.m_using_validation)
			output::log() << info(m_create_info);

		output::dump_logs(std::cout);

		input::key_binding::bind({vkfw::Key::A},
								 [this]() { m_camera.translate_relative(m_camera.right_direction(), -0.1f); });
		input::key_binding::bind({vkfw::Key::D},
								 [this]() { m_camera.translate_relative(m_camera.right_direction(), 0.1f); });
		input::key_binding::bind({vkfw::Key::W},
								 [this]() { m_camera.translate_relative(m_camera.view_direction(), 0.1f); });
		input::key_binding::bind({vkfw::Key::S},
								 [this]() { m_camera.translate_relative(m_camera.view_direction(), -0.1f); });
		input::key_binding::bind({vkfw::Key::Space},
								 [this]() { m_camera.translate_relative(m_camera.up_direction(), 0.1f); });
		input::key_binding::bind({vkfw::Key::C},
								 [this]() { m_camera.translate_relative(m_camera.up_direction(), -0.1f); });

		input::key_binding::bind({vkfw::Key::E}, [this]() { m_camera.look_at(glm::vec3 {0.05f, 0.05f, 0.05f}); });

		input::mouse::move_callback(m_camera.first_person_callback());

		input::key_binding::bind({vkfw::Key::P}, [this]() { m_amb_light2.translate_relative({0.0f, 0.1f, 0.0f}); });
	}

	// ===========================================================================
	// ===========================================================================
	// ===========================================================================

	auto renderer::render() -> void
	{
		m_graphics_queue.command_control().reset();
		m_graphics_queue.add_wait_semaphore(vk::PipelineStageFlagBits::eBottomOfPipe);
		const auto& command_buffer = m_graphics_queue.command_control().front();
		command_buffer.begin({m_graphics_queue.command_control().usage_flags()});

		auto [result,
			  image_index,
			  render_info] = m_swapchain.next_image_info(command_buffer, m_graphics_queue.wait_semaphores().back());

		command_buffer.beginRendering(render_info);

		m_dynamic_rendering_state.state().m_cull_mode = vk::CullModeFlagBits::eNone;
		// m_dynamic_rendering_state.state().m_depth_testing = false;
		m_dynamic_rendering_state.bind(command_buffer);

		glm::ivec4 mi = {0, 1, 2, 3};
		struct test
		{
			glm::mat4x4 model;
			glm::mat4x4 view;
			glm::mat4x4 projection;
			glm::vec4 time;
		};
		struct test2
		{
			glm::mat4x4 model;
			glm::mat4x4 view;
			glm::mat4x4 wtf;
			glm::mat4x4 projection;
			glm::vec4 time;
		};

		auto time = time::now();
		m_point_light.translate_absolute({0.0f, glm::sin(time), 0.0f});

		auto skybox_view_test = m_camera.view();
		skybox_view_test[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		test2 t {
			glm::mat4x4 {1.0f}, m_camera.view(), skybox_view_test, m_camera.projection(), {time, time, time, time}};

		/*
		// full pipeline barrier
		const auto barrier = vk::MemoryBarrier2 {{vk::PipelineStageFlagBits2::eAllCommands},
												 {vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite},
												 {vk::PipelineStageFlagBits2::eAllCommands},
												 {vk::AccessFlagBits2::eMemoryRead |
												  vk::AccessFlagBits2::eMemoryWrite}};
		const auto dependacy_info = vk::DependencyInfo {{}, barrier};
		command_buffer.pipelineBarrier2(dependacy_info);*/

		// test ui vs camera controls
		const auto& imgui_io = ImGui::GetIO();
		if (imgui_io.WantCaptureMouse)
			input::mouse::move_callback([](auto wtf) {});
		else
			input::mouse::move_callback(m_camera.first_person_callback());
		// done testing

		// m_global_descriptor_buffer.flush_resource_descriptors();
		//  m_global_descriptor_buffer.register_resource_buffer(m_test_pipeline.resource_buffer());
		//  m_global_descriptor_buffer.register_resource_buffer(m_skybox.pipeline().resource_buffer());
		//    draw sphere
		m_default_meshes.sphere().vertex_buffer().bind(command_buffer);
		m_test_pipeline.bind(command_buffer);
		m_test_pipeline.resource_buffer().map_uniform_data(0, t);
		m_test_pipeline.resource_buffer().map_uniform_data(1, mi);
		m_test_pipeline.resource_buffer().map_storage_data(0, mi);
		m_test_pipeline.resource_buffer().map_storage_data(1, m_global_light_manager.light_device_addresses());
		m_global_descriptor_buffer.bind(command_buffer);
		// m_global_descriptor_buffer.map_resource_buffer_offsets(command_buffer, m_test_pipeline.resource_buffer());
		command_buffer.drawIndexed(m_default_meshes.sphere().indices().size(), 1, 0, 0, 0);

		// draw skybox
		m_default_meshes.cube().vertex_buffer().bind(command_buffer);
		m_skybox.pipeline().bind(command_buffer);
		//  m_skybox.mesh().vertex_buffer().bind(command_buffer);
		m_skybox.pipeline().resource_buffer().map_uniform_data(0, t);
		// m_global_descriptor_buffer.map_resource_buffer_offsets(command_buffer,
		// m_skybox.pipeline().resource_buffer());
		//   m_global_descriptor_buffer.bind(command_buffer);
		command_buffer.drawIndexed(m_default_meshes.cube().indices().size(), 1, 0, 0, 0);

		m_imgui.new_frame();
		m_imgui.render(command_buffer);

		command_buffer.endRendering();

		// present
		m_swapchain.transition_layout<vulkan::swapchain::layout_state::presentation>(command_buffer);
		command_buffer.end();

		m_graphics_queue.submit_and_wait();

		m_graphics_queue.present(m_swapchain);
	}

	renderer::implementation_inspector::implementation_inspector(const vulkan::instance& instance,
																 const vulkan::physical_device& physical_device,
																 const vulkan::logical_device& logical_device,
																 const vulkan::memory_allocator& memory_allocator)
	{
		auto& descriptor_buffer_properties = const_cast<vulkan::physical_device::physical_properties&>(
												 physical_device.properties())
												 .m_descriptor_buffer_properties;

		// record implementation defined descriptor properties
		auto binding = std::array<vk::DescriptorSetLayoutBinding, 2> {
			vk::DescriptorSetLayoutBinding {0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAll},
			vk::DescriptorSetLayoutBinding {1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAll}};
		const auto basic_uniform_buffer = vk::raii::DescriptorSetLayout {
			*logical_device, {vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT, binding}};

		descriptor_buffer_properties.m_uniform_buffer_size = basic_uniform_buffer.getSizeEXT();
		descriptor_buffer_properties.m_uniform_buffer_offset = basic_uniform_buffer.getBindingOffsetEXT(1);

		binding[0].descriptorType = vk::DescriptorType::eStorageBuffer;
		binding[1].descriptorType = vk::DescriptorType::eStorageBuffer;

		const auto basic_storage_buffer = vk::raii::DescriptorSetLayout {
			*logical_device, {vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT, binding}};

		descriptor_buffer_properties.m_storage_buffer_size = basic_storage_buffer.getSizeEXT();
		descriptor_buffer_properties.m_storage_buffer_offset = basic_storage_buffer.getBindingOffsetEXT(1);

		binding[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		binding[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;

		const auto basic_combined_image_sampler = vk::raii::DescriptorSetLayout {
			*logical_device, {vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT, binding}};

		descriptor_buffer_properties.m_combined_image_sampler_size = basic_combined_image_sampler.getSizeEXT();
		descriptor_buffer_properties.m_combined_image_sampler_offset = basic_combined_image_sampler.getBindingOffsetEXT(
			1);
	}

	auto renderer::info(const create_info& create_info) -> string::string_t
	{
		auto result = string::string_t {"\n======== renderer information: ========\n"};

		result += m_instance.extensions().info();
		result += m_instance.validation_layers().has_value() ? m_instance.validation_layers().value().info()
															 : string::string_t {};
		result += m_physical_device.info();
		result += m_physical_device.extensions().info();
		result += m_logical_device.info();

		return result;
	}
}
