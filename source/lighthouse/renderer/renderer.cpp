module;

#include "vulkan/utils/geometries.hpp"
#include "vulkan/utils/math.hpp"
#include "vkfw/vkfw.hpp"

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/ext.hpp"

#include "vulkan/utils/raii/raii_utils.hpp"

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

module renderer;
import output;
import file_system;

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
		  m_fake_camera {vk::su::createModelViewProjectionClipMatrix(m_surface.extent())},
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

		  e1m4 {m_logical_device, m_queue_families.graphics()},
		  m_transfer_control {m_logical_device, m_queue_families.transfer()},
		  m_queue {m_logical_device, m_queue_families},
		  m_swapchain {m_physical_device, m_logical_device, m_surface, m_queue_families, m_memory_allocator},
		  m_common_descriptor_data {
			  m_logical_device,
			  m_memory_allocator,
			  256, // sizeof(glm::mat4x4) + sizeof(float),
			  vulkan::mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eUniformBuffer |
															 vk::BufferUsageFlagBits::eShaderDeviceAddress,
												  .m_allocation_flags = vma::AllocationCreateFlagBits::eMapped}},

		  m_descriptor_set_layout {m_logical_device,
								   {{0, vk::DescriptorType::eUniformBuffer, 1},
									{1, vk::DescriptorType::eUniformBuffer, 1}}},

		  m_descriptor_buffer {m_physical_device,
							   m_logical_device,
							   m_memory_allocator,
							   m_descriptor_set_layout,
							   vulkan::buffer_subdata {&m_common_descriptor_data, {{0, 64}, {64, 4}}}},

		  m_vertex_buffer {
			  m_logical_device,
			  m_memory_allocator,
			  sizeof(coloredCubeData),
			  vulkan::mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eVertexBuffer |
															 vk::BufferUsageFlagBits::eShaderDeviceAddress,
												  .m_allocation_flags = vma::AllocationCreateFlagBits::eMapped}},
		  m_index_buffer {
			  m_logical_device,
			  m_memory_allocator,
			  sizeof(vulkan::vertex_index_t) * m_col_cube_indices.size(),
			  vulkan::mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eIndexBuffer |
															 vk::BufferUsageFlagBits::eShaderDeviceAddress,
												  .m_allocation_flags = vma::AllocationCreateFlagBits::eMapped}},
		  m_vertex_spirv {input::read_file(file_system::data_path() /= "shaders/basic.vert"),
						  vulkan::spir_v::create_info {.m_shader_stage = vk::ShaderStageFlagBits::eVertex}},

		  m_fragment_spirv {input::read_file(file_system::data_path() /= "shaders/basic.frag"),
							vulkan::spir_v::create_info {.m_shader_stage = vk::ShaderStageFlagBits::eFragment}},
		  m_resource_generator {m_physical_device,
								m_logical_device,
								m_memory_allocator,
								{m_vertex_spirv, m_fragment_spirv}},

		  m_vertex_input_description {m_resource_generator.vertex_input_description()},

		  m_vertex_object {m_logical_device, m_vertex_spirv, m_resource_generator.descriptor_set_layout()},
		  m_fragment_object {m_logical_device, m_fragment_spirv, m_resource_generator.descriptor_set_layout()},

		  m_pipeline_layout {m_logical_device, {{}, **m_descriptor_set_layout}},
		  m_col_cube_data {std::accumulate(std::begin(coloredCubeData),
										   std::end(coloredCubeData),
										   std::vector<vulkan::vertex> {},
										   [](std::vector<vulkan::vertex> vec, VertexPC elem) {
											   vec.push_back({glm::vec4 {elem.x, elem.y, elem.z, elem.w},
															  glm::vec4 {elem.r, elem.g, elem.b, elem.a}});
											   return std::move(vec);
										   })},
		  m_col_cube_indices {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
							  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
		  m_shader_object_pipeline {m_physical_device,
									m_logical_device,
									m_memory_allocator,
									{m_vertex_spirv, m_fragment_spirv}},
		  m_scene_loader {m_logical_device, m_memory_allocator, file_system::data_path() /= "models/xyz.obj"},

		  m_actual_vb {m_logical_device, m_memory_allocator, m_col_cube_data, m_col_cube_indices},
		  // m_camera_node {},
		  m_camera {std::make_shared<lh::node>(), camera<camera_type::perspective>::create_info {}},
		  m_model {1.0f},
		  m_texture {m_logical_device,
					 m_memory_allocator,
					 m_transfer_control,
					 m_queue.transfer(),
					 file_system::data_path() /= "textures/bricks.png"}
	{
		m_vertex_buffer.map_data(*m_col_cube_data.data(), 0, sizeof(vulkan::vertex) * m_col_cube_data.size());
		m_index_buffer.map_data(*m_col_cube_indices.data(),
								0,
								sizeof(vulkan::vertex_index_t) * m_col_cube_indices.size());

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

		input::key_binding::bind({vkfw::Key::T}, [this]() {
			m_model = glm::translate(m_model, glm::vec3 {0.0f, 1.0f, 0.0f} * 0.1f);
		});
		input::key_binding::bind({vkfw::Key::G}, [this]() {
			m_model = glm::translate(m_model, glm::vec3 {0.0f, 1.0f, 0.0f} * -0.1f);
		});
		input::key_binding::bind({vkfw::Key::F}, [this]() {
			m_model = glm::translate(m_model, glm::vec3 {1.0f, 0.0f, 0.0f} * 0.1f);
		});
		input::key_binding::bind({vkfw::Key::H}, [this]() {
			m_model = glm::translate(m_model, glm::vec3 {1.0f, 0.0f, 0.0f} * -0.1f);
		});
		input::key_binding::bind({vkfw::Key::Y}, [this]() {
			m_model = glm::translate(m_model, glm::vec3 {0.0f, 0.0f, 1.0f} * 0.1f);
		});
		input::key_binding::bind({vkfw::Key::U}, [this]() {
			m_model = glm::translate(m_model, glm::vec3 {0.0f, 0.0f, 1.0f} * -0.1f);
		});

		input::key_binding::bind({vkfw::Key::E}, [this]() { m_camera.look_at(glm::vec3 {0.05f, 0.05f, 0.05f}); });

		input::mouse::move_callback(m_camera.first_person_callback());
	}

	auto renderer::render() -> void
	{
		vk::raii::Semaphore semaphore(m_logical_device, vk::SemaphoreCreateInfo());
		const auto& pos = m_camera.position();
		const auto& rot = m_camera.rotation();
		m_window.vkfw_window().setTitle(glm::to_string(pos) + " - " + glm::to_string(rot));
		e1m4.reset();
		const auto& command_buffer = e1m4.first_command_buffer();
		command_buffer.begin({e1m4.usage_flags()});

		auto [result, image_index, render_info] = m_swapchain.next_image_info(command_buffer, semaphore);

		command_buffer.beginRendering(render_info);

		command_buffer.setViewportWithCountEXT(vk::Viewport(0.0f,
															static_cast<float>(m_surface.extent().height),
															static_cast<float>(m_surface.extent().width),
															-static_cast<float>(m_surface.extent().height),
															0.0f,
															1.0f));

		command_buffer.setScissorWithCountEXT(vk::Rect2D(vk::Offset2D(0, 0), m_surface.extent()));
		command_buffer.setCullModeEXT(vk::CullModeFlagBits::eBack);
		command_buffer.setFrontFaceEXT(vk::FrontFace::eClockwise);
		command_buffer.setDepthTestEnableEXT(true);
		command_buffer.setDepthWriteEnableEXT(true);
		command_buffer.setDepthCompareOpEXT(vk::CompareOp::eLessOrEqual);
		command_buffer.setPrimitiveTopologyEXT(vk::PrimitiveTopology::eTriangleList);
		command_buffer.setRasterizerDiscardEnableEXT(false);
		command_buffer.setPolygonModeEXT(vk::PolygonMode::eFill);
		command_buffer.setRasterizationSamplesEXT(vk::SampleCountFlagBits::e1);
		command_buffer.setSampleMaskEXT(vk::SampleCountFlagBits::e1, 1);
		command_buffer.setAlphaToCoverageEnableEXT(false);
		command_buffer.setDepthBiasEnable(false);
		command_buffer.setStencilTestEnable(false);
		command_buffer.setPrimitiveRestartEnable(false);
		command_buffer.setColorBlendEnableEXT(0, {true, false});
		command_buffer.setColorBlendEquationEXT(0,
												{vk::ColorBlendEquationEXT {vk::BlendFactor::eSrcAlpha,
																			vk::BlendFactor::eOneMinusSrcAlpha,
																			vk::BlendOp::eAdd,
																			vk::BlendFactor::eOne,
																			vk::BlendFactor::eZero,
																			vk::BlendOp::eAdd},
												 {}});
		command_buffer.setColorWriteMaskEXT(0,
											{vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
											 vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA});

		// vk::CommandBuffer::setColorBlendEquationEXT()
		/*
		vk::VertexInputBindingDescription2EXT vbd {0, sizeof(VertexPC), vk::VertexInputRate::eVertex, 1};
		std::vector<vk::VertexInputAttributeDescription2EXT> vad {
			vk::VertexInputAttributeDescription2EXT(0, 0, vk::Format::eR32G32B32A32Sfloat, 0),
			vk::VertexInputAttributeDescription2EXT(1, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexPC,
		r))};*/

		// command_buffer.setVertexInputEXT(vbd, vad);

		command_buffer.setVertexInputEXT(m_resource_generator.vertex_input_description().m_bindings,
										 m_resource_generator.vertex_input_description().m_attributes);
		// command_buffer.bindVertexBuffers(0, {**m_vertex_buffer}, {0});
		// command_buffer.bindIndexBuffer(**m_index_buffer, 0, vk::IndexType::eUint32);
		m_scene_loader.meshes()[0].vertex_buffer().bind(command_buffer);

		const auto time = static_cast<float>(vkfw::getTime().value);

		m_fake_camera = glm::rotate(m_fake_camera, glm::sin(time), glm::vec3 {1.0f, 1.0f, 1.0f});

		m_common_descriptor_data.map_data(m_fake_camera);
		m_common_descriptor_data.map_data(sin(time), 64);

		auto model = glm::mat4x4 {1.0f};
		auto view = m_camera.view();

		auto perspective = m_camera.projection();
		glm::mat4x4 clip = glm::mat4x4(
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

		auto test_camera = /*clip **/ perspective * view * m_model;
		m_resource_generator.uniform_buffers().map_data(test_camera);
		m_resource_generator.uniform_buffers().map_data(sin(time), 64);

		m_descriptor_buffer.bind(command_buffer, m_pipeline_layout);
		m_resource_generator.descriptor_buffer().bind(command_buffer, m_resource_generator.pipeline_layout());
		//  ==================

		command_buffer.bindShadersEXT({m_resource_generator.shader_objects()[0].stage(),
									   m_resource_generator.shader_objects()[1].stage()},
									  {**m_resource_generator.shader_objects()[0],
									   **m_resource_generator.shader_objects()[1]});
		/*
		command_buffer.bindShadersEXT({vk::ShaderStageFlagBits::eTessellationControl,
									   vk::ShaderStageFlagBits::eTessellationEvaluation,
									   vk::ShaderStageFlagBits::eGeometry},
									  {nullptr, nullptr, nullptr});*/

		// command_buffer.draw(m_scene_loader.meshes()[0].vertices().size(), 1, 0, 0);
		command_buffer.drawIndexed(m_scene_loader.meshes()[0].indices().size(), 1, 0, 0, 0);
		command_buffer.endRendering();

		// m_swapchain.transition_layout_for_presentation(command_buffer);
		m_swapchain.transition_layout<vulkan::swapchain::layout_state::presentation>(command_buffer);
		command_buffer.end();

		vk::raii::Fence drawFence(m_logical_device, vk::FenceCreateInfo());

		vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
		vk::SubmitInfo submitInfo(*semaphore, waitDestinationStageMask, *command_buffer);
		m_queue.graphics().submit(submitInfo, *drawFence);

		while (vk::Result::eTimeout == m_logical_device->waitForFences({*drawFence}, VK_TRUE, vk::su::FenceTimeout))
			;

		vk::PresentInfoKHR presentInfoKHR(nullptr, **m_swapchain, image_index);
		m_queue.present().presentKHR(presentInfoKHR);

		m_logical_device->waitIdle();
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
