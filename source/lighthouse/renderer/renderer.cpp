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
import collision;

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
		  m_swapchain {m_physical_device, m_logical_device, m_surface, m_queue_families, m_memory_allocator},
		  m_graphics_queue {m_logical_device, m_swapchain, {m_queue_families.graphics()}},
		  m_transfer_queue {m_logical_device, {m_queue_families.transfer()}},
		  m_dynamic_rendering_state {vulkan::dynamic_rendering_state::create_info {
			  .m_viewport = vk::Viewport(0.0f,
										 static_cast<float>(m_surface.extent().height),
										 static_cast<float>(m_surface.extent().width),
										 -static_cast<float>(m_surface.extent().height),
										 0.0f,
										 1.0f),
			  .m_scissor = vk::Rect2D(vk::Offset2D(0, 0), m_surface.extent())}},
		  m_user_interface {window,
							m_instance,
							m_physical_device,
							m_logical_device,
							m_queue_families.graphics(),
							m_graphics_queue,
							m_swapchain},
		  m_global_descriptor {m_physical_device, m_logical_device},
		  m_global_light_manager {m_physical_device, m_logical_device, m_memory_allocator},
		  m_global_descriptor_buffer {m_physical_device, m_logical_device, m_memory_allocator, m_global_descriptor},
		  m_push_constant {},
		  m_mesh_registry {m_logical_device,
						   m_memory_allocator,
						   m_transfer_queue,
						   {file_system::data_path() /= "models/plane.obj",
							file_system::data_path() /= "models/cube.obj",
							file_system::data_path() /= "models/sphere.obj",
							file_system::data_path() /= "models/cylinder.obj",
							file_system::data_path() /= "models/cone.obj"}},
		  m_mapped_range {m_logical_device,
						  m_memory_allocator,
						  256000000 /*std::min(m_physical_device.properties().)*/},
		  m_instance_buffer {m_logical_device, m_memory_allocator, 1000 * sizeof glm::mat4x4},
		  m_test_pipeline {m_physical_device,
						   m_logical_device,
						   m_memory_allocator,
						   {file_system::data_path() /= "shaders/basic.vert",
							file_system::data_path() /= "shaders/basic.frag"},
						   m_global_descriptor,
						   m_global_descriptor_buffer},
		  // m_scene_loader {m_logical_device, m_memory_allocator, file_system::data_path() /= "models/cube.obj"},
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
					m_mesh_registry,
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
		// m_global_descriptor_buffer.register_resource_buffer(m_test_pipeline.resource_buffer());
		// m_global_descriptor_buffer.register_resource_buffer(m_skybox.pipeline().resource_buffer());

		glm::mat4x4 sphere1 = glm::mat4x4 {1.0f};
		glm::mat4x4 sphere2 = glm::translate(sphere1, glm::vec3 {0.0f, 10.0f, 0.0f});
		glm::mat4x4 sphere3 = glm::translate(sphere1, glm::vec3 {0.0f, -10.0f, 0.0f});

		glm::mat4x4 spheres[3] = {sphere1, sphere2, sphere3};

		auto smb = m_instance_buffer.request_and_commit_span<glm::mat4x4>(3);
		// std::cout << "instance buffer address: " << m_instance_buffer.memory_mapped_span::address() << '\n';

		smb.emplace_back(sphere1);
		smb.emplace_back(sphere2);
		smb.emplace_back(sphere3);
		m_push_constant.m_registers.m_address_1 = m_instance_buffer.span_device_address(smb);
		/*m_instance_buffer.address();*/ /*smb.address();*/

		if (m_create_info.m_using_validation) output::log() << info(m_create_info);

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

		input::key_binding::bind({vkfw::Key::E},
								 [this]() { m_camera.look_at(geometry::position_t {0.05f, 0.05f, 0.05f}); });
		/*
		input::key_binding::bind({vkfw::Key::B}, [this]() {
			const auto ray = geometry::ray {m_camera.position(), m_camera.view_direction()};

			if (collision::ray_aabb_test(ray, m_mesh_registry.sphere().bounding_box()))
			{
				std::cout << "found aabb\n";
				for (size_t i = 0; i < m_mesh_registry.sphere().indices().size() - 3; i += 3)
				{
					const auto indices = std::array<uint32_t, 3> {m_mesh_registry.sphere().indices()[i],
																  m_mesh_registry.sphere().indices()[i + 1],
																  m_mesh_registry.sphere().indices()[i + 2]};

					const auto triangle =
						geometry::triangle {m_mesh_registry.sphere().vertices()[indices[0]].m_position,
											m_mesh_registry.sphere().vertices()[indices[1]].m_position,
											m_mesh_registry.sphere().vertices()[indices[2]].m_position};

					if (collision::ray_tri_test(ray, triangle)) std::cout << "got it after: " << i << '\n';
				}
			}
		});*/

		input::mouse::move_callback(m_camera.first_person_callback());

		input::key_binding::bind({vkfw::Key::P}, [this]() { m_amb_light2.translate_relative({0.0f, 0.1f, 0.0f}); });
	}

	auto renderer::push_constants() const -> void
	{
		m_graphics_queue.command_control().front().pushConstants<vulkan::push_constant>(
			m_global_descriptor.pipeline_layout(), vk::ShaderStageFlagBits::eAll, 0, m_push_constant);
	}

	// ===========================================================================
	// ===========================================================================
	// ===========================================================================

	auto renderer::render() -> void
	{
		// m_graphics_queue.wait();
		m_graphics_queue.command_control().reset();

		const auto& command_buffer = m_graphics_queue.command_control().front();
		command_buffer.begin({m_graphics_queue.command_control().usage_flags()});

		auto [result, image_index, render_info, sync_data] = m_swapchain.next_image_info(command_buffer);

		command_buffer.beginRendering(render_info);

		m_dynamic_rendering_state.state().m_cull_mode = vk::CullModeFlagBits::eNone;
		// m_dynamic_rendering_state.state().m_depth_testing = false;
		m_dynamic_rendering_state.bind(command_buffer);
		m_global_descriptor_buffer.bind(command_buffer);

		glm::ivec4 mi = {0, 1, 2, 3};
		struct test
		{
			glm::mat4x4 model[3];
			glm::mat4x4 view;
			glm::mat4x4 projection;
			glm::vec4 time;
		};

		struct test2
		{
			glm::mat4x4 model;
			glm::mat4x4 view;
			glm::mat4x4 projection;
			glm::vec4 time;
		};

		auto time = time::now();
		m_point_light.translate_absolute({0.0f, glm::sin(time), 0.0f});

		auto skybox_view_test = m_camera.view();
		skybox_view_test[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		glm::mat4x4 sphere1 = glm::mat4x4 {1.0f};
		glm::mat4x4 sphere2 = glm::translate(sphere1, glm::vec3 {0.0f, 10.0f, 0.0f});
		glm::mat4x4 sphere3 = glm::translate(sphere1, glm::vec3 {0.0f, -10.0f, 0.0f});
		test scene {{sphere1, sphere2, sphere3}, m_camera.view(), m_camera.projection(), {time, time, time, time}};
		test2 sb_scene = {glm::mat4x4 {1.0f}, m_camera.view(), m_camera.projection(), {time, time, time, time}};
		// auto sb_scene = scene;
		sb_scene.view[3] = glm::vec4 {0.0f, 0.0f, 0.0f, 1.0f};

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
		// imgui_io.ConfigFlags |=
		if (imgui_io.WantCaptureMouse)
			input::mouse::move_callback([](auto wtf) {});
		else
			input::mouse::move_callback(m_camera.first_person_callback());
		// done testing

		// draw sphere

		m_mesh_registry.sphere().bind(command_buffer);
		m_test_pipeline.bind(command_buffer);
		m_test_pipeline.resource_buffer().map_uniform_data(0, scene);
		m_test_pipeline.resource_buffer().map_uniform_data(1, mi);
		m_test_pipeline.resource_buffer().map_storage_data(0, mi);
		m_test_pipeline.resource_buffer().map_storage_data(1, m_global_light_manager.light_device_addresses());

		push_constants();
		command_buffer.drawIndexed(m_mesh_registry.sphere().index_count(), 3, 0, 0, 0);

		/*
		const auto barrier = vk::MemoryBarrier2 {{vk::PipelineStageFlagBits2::eAllCommands},
												 {vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite},
												 {vk::PipelineStageFlagBits2::eAllCommands},
												 {vk::AccessFlagBits2::eMemoryRead |
												  vk::AccessFlagBits2::eMemoryWrite}};
		const auto dependacy_info = vk::DependencyInfo {{}, barrier};
		command_buffer.pipelineBarrier2(dependacy_info);
		*/
		// draw skybox

		m_mesh_registry.cube().bind(command_buffer);
		m_skybox.pipeline().bind(command_buffer);
		m_skybox.pipeline().resource_buffer().map_uniform_data(0, sb_scene);

		push_constants();
		command_buffer.drawIndexed(m_mesh_registry.cube().index_count(), 1, 0, 0, 0);

		// end rendering

		m_user_interface.new_frame();
		m_user_interface.draw_crosshair();
		m_user_interface.draw_gpu_budgets(m_memory_allocator.budget());
		m_user_interface.render(command_buffer);

		command_buffer.endRendering();

		m_swapchain.transition_layout<vulkan::swapchain::layout_state::presentation>(command_buffer);
		command_buffer.end();

		// submit
		m_graphics_queue.add_submit_wait_semaphore(
			{sync_data.m_image_acquired_semaphore, 0, vk::PipelineStageFlagBits2::eBottomOfPipe});
		m_graphics_queue.add_submit_signal_semaphore(
			{sync_data.m_render_finished_semaphore, 0, vk::PipelineStageFlagBits2::eBottomOfPipe});
		m_graphics_queue.submit_and_wait();

		// present
		m_graphics_queue.add_present_wait_semaphore(sync_data.m_render_finished_semaphore);
		m_graphics_queue.present();
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
