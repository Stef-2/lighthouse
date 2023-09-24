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
		  m_global_descriptor {m_physical_device, m_logical_device},
		  m_global_descriptor_buffer {m_physical_device, m_logical_device, m_memory_allocator, m_global_descriptor},
		  m_vertex_spirv {input::read_file(file_system::data_path() /= "shaders/basic.vert")},

		  m_fragment_spirv {input::read_file(file_system::data_path() /= "shaders/basic.frag")},
		  m_resource_generator {m_physical_device,
								m_logical_device,
								m_memory_allocator,
								{file_system::data_path() /= "shaders/basic.vert",
								 file_system::data_path() /= "shaders/basic.frag"},
								m_global_descriptor},
		  m_scene_loader {m_logical_device, m_memory_allocator, file_system::data_path() /= "models/cube.obj"},
		  m_camera {std::make_shared<lh::node>(), camera<camera_type::perspective>::create_info {}},
		  m_texture1 {m_physical_device,
					  m_logical_device,
					  m_memory_allocator,
					  m_transfer_control,
					  m_queue.transfer(),
					  file_system::data_path() /= "textures/grooved_bricks/basecolor.png"},
		  m_texture2 {m_physical_device,
					  m_logical_device,
					  m_memory_allocator,
					  m_transfer_control,
					  m_queue.transfer(),
					  file_system::data_path() /= "textures/grooved_bricks/normal.png"}
	{
		m_global_descriptor_buffer.map_uniform_buffer_data(0,
														   vulkan::buffer_subdata {
															   m_resource_generator.uniform_buffer_subdata()});
		auto wtf = m_global_descriptor_buffer.register_textures({&m_texture1, &m_texture2});
		m_global_descriptor_buffer.unregister_textures({1});

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

		command_buffer.setVertexInputEXT(m_resource_generator.vertex_input_description().m_bindings,
										 m_resource_generator.vertex_input_description().m_attributes);

		m_scene_loader.meshes()[0].vertex_buffer().bind(command_buffer);

		const auto time = static_cast<float>(vkfw::getTime().value);

		auto view = m_camera.view();

		auto perspective = m_camera.projection();
		glm::mat4x4 clip = glm::mat4x4(
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

		glm::ivec4 mi = {0, 1, 2, 3};
		auto test_camera = /*clip **/ perspective * view * glm::mat4x4 {1.0f};
		m_resource_generator.uniform_buffers().map_data(test_camera);
		m_resource_generator.uniform_buffers().map_data(mi, 64);

		m_global_descriptor_buffer.bind(command_buffer, m_global_descriptor.pipeline_layout());
		//  ==================

		m_resource_generator.shader_pipeline().bind(command_buffer);

		command_buffer.drawIndexed(m_scene_loader.meshes()[0].indices().size(), 1, 0, 0, 0);
		command_buffer.endRendering();

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
