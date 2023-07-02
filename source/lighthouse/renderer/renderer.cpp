#include "lighthouse/renderer/renderer.hpp"
#include "lighthouse/input.hpp"

lh::renderer::renderer(const window& window, const create_info& create_info)
	: m_create_info {create_info},
	  m_instance(window,
				 vulkan::instance::create_info {.m_engine_version = create_info.m_engine_version,
												.m_vulkan_version = create_info.m_vulkan_version}),
	  m_physical_device {m_instance},
	  m_surface {window, m_instance, m_physical_device},
	  m_queue_families {m_physical_device, m_surface},
	  m_logical_device {m_physical_device,
						vulkan::logical_device::create_info {
							.m_queues = {vk::DeviceQueueCreateInfo {
								{}, m_queue_families.graphics().m_index, 1, &m_queue_families.graphics().m_priority}},
							.m_extensions = m_physical_device.extensions().required_extensions()}},
	  m_memory_allocator {m_instance, m_physical_device, m_logical_device},

	  m_command_control {m_logical_device, m_queue_families},
	  m_queue {m_logical_device, m_queue_families},
	  m_swapchain {m_physical_device, m_logical_device, m_surface, m_queue_families, m_memory_allocator},
	  m_common_descriptor_data {
		  m_physical_device,
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
		  m_physical_device,
		  m_logical_device,
		  m_memory_allocator,
		  sizeof(coloredCubeData),
		  vulkan::mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eVertexBuffer |
														 vk::BufferUsageFlagBits::eShaderDeviceAddress,
											  .m_allocation_flags = vma::AllocationCreateFlagBits::eMapped}},
	  m_vertex_spirv {lh::input::read_file(file_system::data_path() /= "shaders/basic.vert"),
					  vulkan::spir_v::create_info {.m_shader_stages = vk::ShaderStageFlagBits::eVertex}},

	  m_fragment_spirv {lh::input::read_file(file_system::data_path() /= "shaders/basic.frag"),
						vulkan::spir_v::create_info {.m_shader_stages = vk::ShaderStageFlagBits::eFragment}},
	  m_vertex_input_description {m_vertex_spirv.reflect_shader_input().vertex_input_description()},

	  m_vertex_object {m_logical_device, m_vertex_spirv, m_descriptor_set_layout},
	  m_fragment_object {m_logical_device, m_fragment_spirv, m_descriptor_set_layout},

	  m_pipeline_layout {m_logical_device, {{}, **m_descriptor_set_layout}},
	  m_shader_object_pipeline {m_physical_device,
								m_logical_device,
								m_memory_allocator,
								{m_vertex_spirv, m_fragment_spirv}},
	  m_new_vb {m_physical_device, m_logical_device, m_memory_allocator, {}}
{
	m_vertex_buffer.map_data(coloredCubeData);

	if (m_create_info.m_using_validation)
		output::log() << info(m_create_info);

	m_new_vb = vulkan::vertex_buffer {m_physical_device, m_logical_device, m_memory_allocator, {}};
}

auto lh::renderer::render() -> void
{
	vk::raii::Semaphore semaphore(m_logical_device, vk::SemaphoreCreateInfo());

	m_command_control.reset();
	const auto& command_buffer = m_command_control.first_command_buffer();
	command_buffer.begin({m_command_control.usage_flags()});

	auto [result, image_index, render_info] = m_swapchain.next_image_info(command_buffer, semaphore);

	command_buffer.beginRendering(render_info);

	command_buffer.setViewportWithCountEXT(vk::Viewport(0.0f,
														0.0f,
														static_cast<float>(m_surface.extent().width),
														static_cast<float>(m_surface.extent().height),
														0.0f,
														1.0f));

	command_buffer.setScissorWithCountEXT(vk::Rect2D(vk::Offset2D(0, 0), m_surface.extent()));
	command_buffer.setCullModeEXT(vk::CullModeFlagBits::eBack);
	command_buffer.setFrontFaceEXT(vk::FrontFace::eClockwise);
	command_buffer.setDepthTestEnableEXT(true);
	command_buffer.setDepthWriteEnableEXT(true);
	command_buffer.setDepthCompareOpEXT(vk::CompareOp::eLessOrEqual);
	command_buffer.setPrimitiveTopologyEXT(vk::PrimitiveTopology::eTriangleList);

	vk::VertexInputBindingDescription2EXT vbd {0, sizeof(VertexPC), vk::VertexInputRate::eVertex, 1};
	std::vector<vk::VertexInputAttributeDescription2EXT> vad {
		vk::VertexInputAttributeDescription2EXT(0, 0, vk::Format::eR32G32B32A32Sfloat, 0),
		vk::VertexInputAttributeDescription2EXT(1, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(VertexPC, r))};

	// command_buffer.setVertexInputEXT(vbd, vad);

	command_buffer.setVertexInputEXT(m_vertex_input_description.m_bindings, m_vertex_input_description.m_attributes);
	command_buffer.bindVertexBuffers(0, {**m_vertex_buffer}, {0});

	const auto time = static_cast<float>(vkfw::getTime().value);
	glm::mat4x4 mvpcMatrix = vk::su::createModelViewProjectionClipMatrix(m_surface.extent());
	mvpcMatrix = glm::rotate(mvpcMatrix, glm::sin(time), glm::vec3 {1.0f, 1.0f, 1.0f});

	m_common_descriptor_data.map_data(mvpcMatrix);
	m_common_descriptor_data.map_data(sin(time), 64);

	m_descriptor_buffer.bind(command_buffer, m_pipeline_layout);

	// ==================

	command_buffer.bindShadersEXT({vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment},
								  {**m_vertex_object, **m_fragment_object});

	command_buffer.bindShadersEXT({m_vertex_object.stage(), m_fragment_object.stage()},
								  {**m_vertex_object, **m_fragment_object});
	/*
	command_buffer.bindShadersEXT({vk::ShaderStageFlagBits::eTessellationControl,
								   vk::ShaderStageFlagBits::eTessellationEvaluation,
								   vk::ShaderStageFlagBits::eGeometry},
								  {nullptr, nullptr, nullptr});*/

	command_buffer.draw(12 * 3, 1, 0, 0);
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

auto lh::renderer::info(const create_info& create_info) -> string::string_t
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
