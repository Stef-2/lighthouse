#include "lighthouse/renderer.hpp"
#include "lighthouse/input.hpp"
// #include "vulkan/vma/vk_mem_alloc.h"

lh::renderer::renderer(const window& window, const create_info& create_info)
	: m_instance(window),
	  m_physical_device {m_instance},
	  m_surface {window, m_instance, m_physical_device},
	  m_queue_families {m_physical_device, m_surface},
	  m_device {m_physical_device,
				vulkan::logical_device::create_info {
					.m_queues = {vk::DeviceQueueCreateInfo {
						{}, m_queue_families.graphics().m_index, 1, &m_queue_families.graphics().m_priority}},
					.m_extensions = m_physical_device.extensions().required_extensions()}},
	  m_memory_allocator {m_instance, m_physical_device, m_device},

	  m_command_control {m_device, m_queue_families},
	  m_queue {m_device, m_queue_families},
	  m_dyn_rend_image {m_physical_device, m_device, m_memory_allocator, m_surface.extent()},
	  m_swapchain {m_physical_device, m_device, m_surface, m_queue_families, m_memory_allocator},

	  m_uniform_buffer {m_physical_device,
						m_device,
						m_memory_allocator,
						sizeof(glm::mat4x4),
						vulkan::mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eUniformBuffer |
																	   vk::BufferUsageFlagBits::eShaderDeviceAddress}},
	  m_descriptor_set_layout {m_device,
							   vulkan::descriptor_set_layout::create_info {
								   .m_flags = {},
								   .m_bindings = {{0, vk::DescriptorType::eUniformBuffer, 1, sizeof(glm::mat4x4)}}}},
	  m_temp_buffered_dsl {m_device,
						   vulkan::descriptor_set_layout::create_info {
							   .m_bindings = {{0, vk::DescriptorType::eUniformBuffer, 1, sizeof(glm::mat4x4)}}}},

	  m_descriptor_collection {m_physical_device, m_device, m_temp_buffered_dsl, m_memory_allocator},

	  m_vertex_buffer {create_vertex_buffer()},

	  m_vertex_object {m_device,
					   vulkan::spir_v {lh::input::read_file(file_system::data_path() /= "shaders/basic.vert"),
									   vulkan::spir_v::create_info {
										   .m_shader_stages = vk::ShaderStageFlagBits::eVertex}},
					   m_temp_buffered_dsl},
	  m_fragment_object {m_device,
						 vulkan::spir_v {lh::input::read_file(file_system::data_path() /= "shaders/basic.frag"),
										 vulkan::spir_v::create_info {
											 .m_shader_stages = vk::ShaderStageFlagBits::eFragment}},
						 m_temp_buffered_dsl},

	  m_pipeline_layout {create_pipeline_layout()}
{
	if (create_info.m_using_validation)
		output::log() << info();
}

auto lh::renderer::create_depth_buffer(const window& window) -> vk::raii::ImageView
{
	const auto depth_format = vk::Format::eB8G8R8A8Srgb;
	auto format_properties = vk::raii::PhysicalDevice(m_physical_device).getFormatProperties(depth_format);

	auto tiling_mode = vk::ImageTiling {};

	if (format_properties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
	{
		tiling_mode = vk::ImageTiling::eLinear;
	} else if (format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
	{
		tiling_mode = vk::ImageTiling::eOptimal;
	} else
	{
		output::fatal() << "DepthStencilAttachment is not supported for D16Unorm depth format";
	}

	auto image_info = vk::ImageCreateInfo {};
	image_info.imageType = vk::ImageType::e2D;
	image_info.format = depth_format;
	image_info.extent = vk::Extent3D {window.resolution().width, window.resolution().height, 1};
	image_info.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;

	auto image = vk::raii::Image {m_device, image_info};

	auto memory_properties = vk::raii::PhysicalDevice(m_physical_device).getMemoryProperties();
	auto memory_requirements = image.getMemoryRequirements();

	auto type_bits = memory_requirements.memoryTypeBits;
	auto type_index = uint32_t(~0);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if ((type_bits & 1) && ((memory_properties.memoryTypes[i].propertyFlags &
								 vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal))
		{
			type_index = i;
			break;
		}
		type_bits >>= 1;
	}

	auto memory_info = vk::MemoryAllocateInfo(memory_requirements.size, type_index);
	auto image_memory = vk::raii::DeviceMemory(m_device, memory_info);
	image.bindMemory(*image_memory, 0);

	auto image_view_info = vk::ImageViewCreateInfo(
		{}, *image, vk::ImageViewType::e2D, depth_format, {}, {vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});

	return {m_device, image_view_info};
}

auto lh::renderer::create_depth_buffer_data(const window&) -> vk::raii::su::DepthBufferData
{
	return {m_physical_device, m_device, vk::Format::eD16Unorm, m_surface.extent()};
}

auto lh::renderer::create_uniform_buffer() -> vk::raii::su::BufferData
{
	vk::raii::su::BufferData uniformBufferData(m_physical_device,
											   m_device,
											   sizeof(glm::mat4x4),
											   vk::BufferUsageFlagBits::eUniformBuffer);
	glm::mat4x4 mvpcMatrix = vk::su::createModelViewProjectionClipMatrix(m_surface.extent());
	vk::raii::su::copyToDevice(uniformBufferData.deviceMemory, mvpcMatrix);

	return uniformBufferData;
}

auto lh::renderer::create_descriptor_set_layout() -> vk::raii::DescriptorSetLayout
{
	return vk::raii::su::makeDescriptorSetLayout(
		m_device, {{vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAll}});
}

auto lh::renderer::create_pipeline_layout() -> vk::raii::PipelineLayout
{
	return {m_device, {{}, **m_temp_buffered_dsl}};
}

auto lh::renderer::create_format() -> vk::Format
{
	return vk::su::pickSurfaceFormat(vk::raii::PhysicalDevice(m_physical_device).getSurfaceFormatsKHR(**m_surface))
		.format;
}

auto lh::renderer::create_pipeline_cache() -> vk::raii::PipelineCache
{
	return {m_device, vk::PipelineCacheCreateInfo {}};
}

auto lh::renderer::create_vertex_buffer() -> vk::raii::su::BufferData
{

	vk::raii::su::BufferData vertexBufferData(m_physical_device,
											  m_device,
											  sizeof(coloredCubeData),
											  vk::BufferUsageFlagBits::eVertexBuffer);
	vk::raii::su::copyToDevice(vertexBufferData.deviceMemory,
							   coloredCubeData,
							   sizeof(coloredCubeData) / sizeof(coloredCubeData[0]));

	return vertexBufferData;
}

// ============================================================================
#pragma optimize("", off)
auto lh::renderer::render() -> void
{
	vk::raii::Semaphore semaphore(m_device, vk::SemaphoreCreateInfo());

	const auto& command_buffer = m_command_control.command_buffers().front();

	command_buffer.begin({});

	auto [result, image_index, render_info] = m_swapchain.next_image_info(command_buffer, semaphore);

	command_buffer.beginRendering(render_info);

	// command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);

	// shader object setup
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

	command_buffer.setVertexInputEXT(vbd, vad);
	command_buffer.bindVertexBuffers(0, {*m_vertex_buffer.buffer}, {0});

	//    ==================
	const auto desc_buffer =
		vk::DescriptorBufferBindingInfoEXT {m_descriptor_collection.descriptor_buffers()[0].address(),
											vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT};
	glm::mat4x4 mvpcMatrix = vk::su::createModelViewProjectionClipMatrix(m_surface.extent());
	mvpcMatrix = glm::rotate(mvpcMatrix, float(glm::sin(vkfw::getTime().value)), glm::vec3 {1.0f, 1.0f, 1.0f});
	m_descriptor_collection.data_buffers()[0].map_data(mvpcMatrix);

	command_buffer.bindDescriptorBuffersEXT(desc_buffer);
	command_buffer.setDescriptorBufferOffsetsEXT(vk::PipelineBindPoint::eGraphics, *m_pipeline_layout, 0, {0}, {0});

	// ==================

	command_buffer.bindShadersEXT({vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment},
								  {**m_vertex_object, **m_fragment_object});
	/*
	command_buffer.bindShadersEXT({vk::ShaderStageFlagBits::eTessellationControl,
								   vk::ShaderStageFlagBits::eTessellationEvaluation,
								   vk::ShaderStageFlagBits::eGeometry},
								  {nullptr, nullptr, nullptr});*/

	command_buffer.draw(12 * 3, 1, 0, 0);
	command_buffer.endRendering();

	m_swapchain.transition_layout_for_presentation(command_buffer);
	command_buffer.end();

	vk::raii::Fence drawFence(m_device, vk::FenceCreateInfo());

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
	vk::SubmitInfo submitInfo(*semaphore, waitDestinationStageMask, *command_buffer);
	m_queue.graphics().submit(submitInfo, *drawFence);

	while (vk::Result::eTimeout == m_device->waitForFences({*drawFence}, VK_TRUE, vk::su::FenceTimeout))
		;

	vk::PresentInfoKHR presentInfoKHR(nullptr, **m_swapchain, image_index);
	m_queue.present().presentKHR(presentInfoKHR);

	m_device->waitIdle();
}

auto lh::renderer::info(const create_info& create_info) -> string::string_t
{
	auto result = string::string_t {"\n======== renderer information: ========\n"};

	result += m_instance.extensions().info();
	result += m_instance.validation_layers().value().info();
	result += m_physical_device.info();
	result += m_physical_device.extensions().info();
	result += m_device.info();

	return result;
}
