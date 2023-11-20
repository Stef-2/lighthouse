module;

#include "glm/mat4x4.hpp"
#include "glm/ext.hpp"

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

module renderer;

import output;
import file_system;
import time;

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
							vulkan::logical_device::create_info {
								.m_queues = {vk::DeviceQueueCreateInfo {{},
																		m_queue_families.graphics().m_index,
																		1,
																		&m_queue_families.graphics().m_priority},
											 vk::DeviceQueueCreateInfo {{},
																		m_queue_families.transfer().m_index,
																		1,
																		&m_queue_families.transfer().m_priority}},
								.m_extensions = m_physical_device.extensions().required_extensions()}},
		  m_memory_allocator {m_instance, m_physical_device, m_logical_device},
		  m_implementation_inspector(m_instance, m_physical_device, m_logical_device, m_memory_allocator),
		  e1m4 {m_logical_device, m_queue_families.graphics()},
		  m_transfer_control {m_logical_device, m_queue_families.transfer()},
		  m_queue {m_logical_device, m_queue_families},
		  m_swapchain {m_physical_device, m_logical_device, m_surface, m_queue_families, m_memory_allocator},
		  m_dynamic_rendering_state {vulkan::dynamic_rendering_state::create_info {
			  .m_viewport = vk::Viewport(0.0f,
										 static_cast<float>(m_surface.extent().height),
										 static_cast<float>(m_surface.extent().width),
										 -static_cast<float>(m_surface.extent().height),
										 0.0f,
										 1.0f),
			  .m_scissor = vk::Rect2D(vk::Offset2D(0, 0), m_surface.extent())}},
		  m_global_descriptor {m_physical_device, m_logical_device},
		  m_global_light_descriptor_buffer {m_physical_device, m_logical_device, m_memory_allocator},
		  m_global_descriptor_buffer {m_physical_device,
									  m_logical_device,
									  m_memory_allocator,
									  m_global_descriptor,
									  m_global_light_descriptor_buffer},
		  m_resource_generator {m_physical_device,
								m_logical_device,
								m_memory_allocator,
								{file_system::data_path() /= "shaders/basic.vert",
								 file_system::data_path() /= "shaders/basic.frag"},
								m_global_descriptor},
		  m_scene_loader {m_logical_device, m_memory_allocator, file_system::data_path() /= "models/cube.obj"},
		  m_camera {std::make_shared<lh::node>(), camera<camera_type::perspective>::create_info {}},
		  m_material {m_physical_device,
					  m_logical_device,
					  m_memory_allocator,
					  m_transfer_control,
					  m_queue.transfer(),
					  {file_system::data_path() /= "textures/grooved_bricks/basecolor.png",
					   file_system::data_path() /= "textures/grooved_bricks/normal.png",
					   file_system::data_path() /= "textures/grooved_bricks/ambientocclusion.png"}},
		  m_point_light {{1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {1.0f, 0.0f, 0.0f}},
		  m_point_light2 {{0.0f, 1.0f, 0.0f, 1.0f}, 1.0f, {0.0f, 1.0f, 0.0f}},
		  m_spot_light {{0.5f, 0.5f, 0.0f, 1.0f}, 1.0f, {0.0f, 0.0f, 1.0f}},
		  m_spot_light2 {{0.0f, 0.5f, 0.5f, 1.0f}, 1.0f, {0.0f, 0.0f, 1.0f}},
		  m_dir_light {{1.0f, 0.5f, 0.5f, 1.0f}, 1.0f, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		  m_dir_light2 {{0.5f, 1.0f, 0.5f, 1.0f}, 1.0f, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		  m_amb_light {{1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {1.0f, 0.0f, 0.0f}},
		  m_amb_light2 {{0.0f, 1.0f, 0.0f, 1.0f}, 0.5f, {0.0f, 0.0f, 0.0f}}
	{
		m_global_descriptor_buffer.map_resource_buffer(m_resource_generator.descriptor_buffer());
		m_global_descriptor_buffer.map_material(m_material);

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

	auto renderer::render() -> void
	{
		vk::raii::Semaphore semaphore(m_logical_device, vk::SemaphoreCreateInfo());

		e1m4.reset();
		const auto& command_buffer = e1m4.first_command_buffer();
		command_buffer.begin({e1m4.usage_flags()});

		auto [result, image_index, render_info] = m_swapchain.next_image_info(command_buffer, semaphore);

		command_buffer.beginRendering(render_info);

		m_dynamic_rendering_state.bind(command_buffer);
		m_scene_loader.meshes()[0].vertex_buffer().bind(command_buffer);

		glm::ivec4 mi = {0, 1, 2, 3};
		struct test
		{
			glm::mat4x4 x;
			glm::vec4 t;
		};

		struct pl
		{
			glm::vec4 position;
			glm::vec4 color;
		};

		auto test_camera = m_camera.projection() * m_camera.view() * glm::mat4x4 {1.0f};
		auto wtf = time::now();

		test t {test_camera, {wtf, 0, 0, 0}};
		pl _pl {{1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}};

		m_resource_generator.descriptor_buffer().map_uniform_data(0, t);
		m_resource_generator.descriptor_buffer().map_uniform_data(1, mi);
		m_resource_generator.descriptor_buffer().map_storage_data(0, mi);

		// m_global_light_descriptor_buffer.light_resource_buffer().mapped_buffer().map_data(_pl);
		// m_global_light_descriptor_buffer.light_resource_buffer().map_storage_data(0, _pl);

		m_global_descriptor_buffer.bind(command_buffer, m_global_descriptor.pipeline_layout());
		//  ==================

		m_resource_generator.bind(command_buffer);

		command_buffer.drawIndexed(m_scene_loader.meshes()[0].indices().size(), 1, 0, 0, 0);
		command_buffer.endRendering();

		m_swapchain.transition_layout<vulkan::swapchain::layout_state::presentation>(command_buffer);
		command_buffer.end();

		vk::raii::Fence drawFence(m_logical_device, vk::FenceCreateInfo());

		vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
		vk::SubmitInfo submitInfo(*semaphore, waitDestinationStageMask, *command_buffer);
		m_queue.graphics().submit(submitInfo, *drawFence);

		while (vk::Result::eTimeout == m_logical_device->waitForFences({*drawFence}, true, 1000000))
			;

		vk::PresentInfoKHR presentInfoKHR(nullptr, **m_swapchain, image_index);
		std::ignore = m_queue.present().presentKHR(presentInfoKHR);

		// m_logical_device->waitIdle();
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
