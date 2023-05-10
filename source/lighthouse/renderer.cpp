#include "lighthouse/renderer.hpp"
#include "lighthouse/input.hpp"
// #include "vulkan/vma/vk_mem_alloc.h"

lh::renderer::renderer(const window& window, const create_info& create_info)
	: // m_version {create_info.m_vulkan_version},
	  //  m_context {create_context()},
	  m_instance(window),
	  /*m_validation_module {create_info.m_using_validation ? validation_module {m_instance}
														  : decltype(m_validation_module) {std::nullopt}},*/
	  m_physical_device {m_instance},
	  // m_surface_data {create_surface_data(window)},
	  // m_extent {create_extent(window)},
	  m_surface {window, m_instance, m_physical_device},
	  m_queue_families {m_physical_device, m_surface},
	  // m_physical_extensions {m_physical_device},
	  m_device {m_physical_device,
				vulkan::logical_device::create_info {
					.m_queues = {vk::DeviceQueueCreateInfo {{}, m_queue_families.graphics_index(), 1}},
					.m_extensions = m_physical_device.extensions().required_extensions()}},
	  m_memory_allocator {m_instance, m_physical_device, m_device},
	  // m_command_pool {create_command_pool()},
	  // m_graphics_queue {create_graphics_queue()},
	  // m_present_queue {create_present_queue()},w
	  // m_command_buffer {create_command_buffer()},
	  m_command_control {m_device, m_queue_families},
	  m_queue {m_device, m_queue_families},
	  m_dyn_rend_image {m_physical_device, m_device, m_memory_allocator, m_surface.extent()},
	  m_new_swapchain {m_physical_device, m_device, m_surface, m_queue_families, m_memory_allocator},
	  // m_swapchain {create_swapchain(window)},
	  m_swapchain {m_physical_device, m_device, m_surface, m_queue_families, m_memory_allocator, m_renderpass},
	  // m_swapchain_data {create_swapchain_data(window)},
	  //  m_image_views {create_image_views()},
	  //  m_depth_buffer {create_depth_buffer(window)},
	  // m_depth_buffer_data {create_depth_buffer_data(window)},
	  /*m_depth_buffer(
		  m_physical_device, m_device, m_memory_allocator, window.get_resolution(), depth_buffer::m_defaults),*/
	  m_uniform_buffer {m_physical_device,
						m_device,
						m_memory_allocator,
						sizeof(glm::mat4x4),
						vulkan::buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eUniformBuffer}},
	  m_descriptor_set_layout {m_device,
							   vulkan::descriptor_set_layout::create_info {
								   .m_bindings = {{0, vk::DescriptorType::eUniformBuffer}}}},
	  // m_descriptor_set_layout {create_descriptor_set_layout()},
	  // m_format {create_format()},
	  m_descriptor_set {create_descriptor_set()},
	  m_renderpass {m_physical_device, m_device, m_surface},
	  m_vertex_buffer {create_vertex_buffer()},
	  // m_framebuffers {create_framebuffers(window)},
	  m_descriptor_pool {create_descriptor_pool()},
	  /*
	  m_shader_modules {create_shader_module(vk::ShaderStageFlagBits::eVertex),
						create_shader_module(vk::ShaderStageFlagBits::eFragment)},*/
	  m_vertex {m_device,
				lh::input::read_file(file_system::data_path() /= "shaders/basic.vert"),
				vulkan::shader_module::create_info {.m_shader_stages = vk::ShaderStageFlagBits::eVertex}},
	  m_fragment {m_device,
				  lh::input::read_file(file_system::data_path() /= "shaders/basic.frag"),
				  vulkan::shader_module::create_info {.m_shader_stages = vk::ShaderStageFlagBits::eFragment}},
	  m_pipeline_layout {create_pipeline_layout()},
	  m_pipeline_cache {create_pipeline_cache()},
	  m_pipeline {create_pipeline()}

{
	if (create_info.m_using_validation)
		output::log() << info();
}

auto lh::renderer::logical_extension_module::assert_required_extensions() const -> bool
{
	// make sure the implementation supports all required extensions
	auto required_extensions = logical_extension_module::required_extensions();
	auto supported_extensions = logical_extension_module::supported_extensions();

	for (const auto& required : required_extensions)
	{
		if (std::find_if(supported_extensions.begin(), supported_extensions.end(), [&required](const auto& supported) {
				return strcmp(required, supported.extensionName) == 0;
			}) == supported_extensions.end())
		{
			output::error() << "this system does not support the required vulkan logical extension: " +
								   std::string {required};
			return false;
		}
	}
	return true;
}

auto lh::renderer::logical_extension_module::info() const -> output::string_t
{
	const auto supported_extensions = logical_extension_module::supported_extensions();
	const auto required_extensions = logical_extension_module::required_extensions();

	const auto supported = std::accumulate(supported_extensions.begin(),
										   supported_extensions.end(),
										   output::string_t {"\n======== supported logical extensions: ========\n"},
										   [](const auto& x, const auto& y) {
											   return std::move(x) + '\t' + static_cast<const char*>(y.extensionName) +
													  '\n';
										   });

	const auto required = std::accumulate(required_extensions.begin(),
										  required_extensions.end(),
										  output::string_t {"\n======== required logical extensions: ========\n"},
										  [](const auto& x, const auto& y) {
											  return std::move(x) + '\t' + static_cast<const char*>(y) + '\n';
										  });

	return supported + required;
}

auto lh::renderer::validation_module::assert_required_validation_layers() -> bool
{
	// make sure the implementation supports all required validation layers
	auto required_layers = validation_module::required_validation_layers();
	auto supported_layers = validation_module::supported_validation_layers();

	for (const auto& required : required_layers)
	{
		if (std::find_if(supported_layers.begin(), supported_layers.end(), [&required](const auto& supported) {
				return strcmp(required, supported.layerName) == 0;
			}) == supported_layers.end())
		{
			output::error() << "this system does not support the required vulkan validation layer: " +
								   std::string {required};
			return false;
		}
	}

	return true;
}

auto lh::renderer::validation_module::info() const -> output::string_t
{
	const auto supported_layers = supported_validation_layers();
	const auto required_layers = required_validation_layers();

	const auto supported = std::accumulate(supported_layers.begin(),
										   supported_layers.end(),
										   output::string_t {"\n======== supported validation layers: ========\n"},
										   [](const auto& x, const auto& y) {
											   return std::move(x) + '\t' + static_cast<const char*>(y.layerName) +
													  '\n';
										   });

	const auto required = std::accumulate(required_layers.begin(),
										  required_layers.end(),
										  output::string_t {"\n======== required validation layers: ========\n"},
										  [](const auto& x, const auto& y) {
											  return std::move(x) + '\t' + static_cast<const char*>(y) + '\n';
										  });

	return supported + required;
}

auto lh::renderer::logical_extension_module::supported_extensions() const -> vulkan::vk_extensions_t
{
	// find the number of supported extensions first
	auto num_extensions = uint32_t {0};
	vk::enumerateInstanceExtensionProperties(nullptr, &num_extensions, nullptr);

	auto extensions = std::vector<vk::ExtensionProperties>(num_extensions);
	vk::enumerateInstanceExtensionProperties(nullptr, &num_extensions, extensions.data());

	return extensions;
}

auto lh::renderer::validation_module::supported_validation_layers() const -> std::vector<vk::LayerProperties>
{
	// find the number of supported layers first
	auto num_layers = uint32_t {0};
	vk::enumerateInstanceLayerProperties(&num_layers, nullptr);

	auto layers = std::vector<vk::LayerProperties>(num_layers);
	vk::enumerateInstanceLayerProperties(&num_layers, layers.data());

	return layers;
}

auto lh::renderer::logical_extension_module::required_extensions() const -> vulkan::vk_string_t
{
	// combine the extensions required by glfw with those specified in m_required_extensions
	auto num_extensions = uint32_t {0};
	auto glfw_extensions = vkfw::getRequiredInstanceExtensions(&num_extensions);

	auto combined_extensions = vulkan::vk_string_t {glfw_extensions, glfw_extensions + num_extensions};
	combined_extensions.insert(combined_extensions.end(),
							   m_defaults.m_required_extensions.begin(),
							   m_defaults.m_required_extensions.end());

	return combined_extensions;
}

lh::renderer::validation_module::validation_module(vk::raii::Instance& instance)
{
	m_object = {instance.createDebugUtilsMessengerEXT(m_defaults.m_debug_info)};
}

auto lh::renderer::validation_module::required_validation_layers() const -> vulkan::vk_string_t
{
	return m_defaults.m_required_validation_layers;
}

auto lh::renderer::create_context() -> vk::raii::Context
{
	return {};
}
/*
auto lh::renderer::create_instance(const window& window,
								   const version& engine_version,
								   const version& vulkan_version,
								   bool use_validation_module) -> vk::raii::Instance
{
	// make sure both logical extensions and validation layers checks passed
	if (auto validation_module = use_validation_module ? m_validation_module->assert_required_validation_layers()
													   : true;
		!(m_logical_extensions.assert_required_extensions() && validation_module))
	{
		output::fatal() << "this system does not support the required vulkan components";
	}

	const auto required_extensions = m_logical_extensions.required_extensions();
	const auto required_validation_layers = use_validation_module ? m_validation_module->required_validation_layers()
																  : vulkan::vk_string_t {};

	const auto& instance_debugger = use_validation_module ? &validation_module::m_defaults.m_debug_info : nullptr;

	const auto app_info = vk::ApplicationInfo {
		window.get_title().data(), engine_version, window.get_title().data(), engine_version, vulkan_version};

	const auto instance_info = vk::InstanceCreateInfo {{},
													   &app_info,
													   static_cast<uint32_t>(required_validation_layers.size()),
													   required_validation_layers.data(),
													   static_cast<uint32_t>(required_extensions.size()),
													   required_extensions.data(),
													   instance_debugger};

	return {m_context, instance_info};
}*/
/*
auto lh::renderer::create_device(const vk::PhysicalDeviceFeatures2& features) -> vk::raii::Device
{
	// physical extensions
	const auto required_extensions = m_physical_device.required_extensions();

	if (!m_physical_device.assert_required_extensions())
		output::fatal() << "this system does not support the required vulkan components";

	const auto queue_priority = 0.0f;

	const auto device_queue_info = vk::DeviceQueueCreateInfo {{}, m_queue_families.m_graphics, 1, &queue_priority};
	auto device_info = vk::DeviceCreateInfo {{}, device_queue_info, {}, required_extensions, &features.features};

	return {m_physical_device, device_info};

}
*/
/*
auto lh::renderer::create_command_pool() -> vk::raii::CommandPool
{
	// create a command pool to allocate a command buffer from
	auto command_pool_info = vk::CommandPoolCreateInfo {};
	command_pool_info.queueFamilyIndex = m_graphics_family_queue_indices.first;


	return {m_device, {vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_queue_families.m_graphics}};
}
*/
auto lh::renderer::create_graphics_queue() -> vk::raii::Queue
{
	return {m_device, m_queue_families.graphics_index(), 0};
}

auto lh::renderer::create_present_queue() -> vk::raii::Queue
{
	return {m_device, m_queue_families.present_index(), 0};
}
/*
auto lh::renderer::create_swapchain(const vk::raii::PhysicalDevice& device, window& window) -> swapchain
{
  const auto physical_device = *m_physical_device;

  const auto capabilities = physical_device.getSurfaceCapabilities2KHR(*m_surface);
  const auto formats = physical_device.getSurfaceFormats2KHR(*m_surface);
  const auto present_modes = physical_device.getSurfacePresentModesKHR(*m_surface);

  auto extent = window.get_resolution();
  auto format = swapchain::defaults::m_format;
  auto present_mode = swapchain::defaults::m_present_mode;
  auto image_count = swapchain::defaults::m_image_count;
  auto image_usage = swapchain::defaults::m_image_usage;
  auto sharing_mode = swapchain::defaults::m_sharing_mode;
  auto transform = swapchain::defaults::m_transform;
  auto alpha = swapchain::defaults::m_alpha;

  auto queue_family_indices = {m_queue_families.m_graphics, m_queue_families.m_present};

  // assert that the system supports any formats and present modes
  if (formats.empty() || present_modes.empty())
	output::fatal() << "this system does not meet the minimal vulkan requirements";

  // attempt to acquire the prefered surface format, if unavailable, take the first one that is
  if (!std::ranges::contains(formats, swapchain::defaults::m_format))
  {
	output::warning() << "this system does not support the prefered vulkan surface format";
	format = formats.front();
  }

  // attempt to acquire the prefered present mode, if unavailable, default to FIFO
  if (!std::ranges::contains(present_modes, swapchain::defaults::m_present_mode))
  {
	output::warning() << "this system does not support the prefered vulkan present mode";
	present_mode = vk::PresentModeKHR::eFifo;
  }

  // clamp the swapchain extent between the minimum and maximum supported by implementation
  extent.width = std::clamp(extent.width, capabilities.surfaceCapabilities.minImageExtent.width,
							capabilities.surfaceCapabilities.maxImageExtent.width);
  extent.height = std::clamp(extent.height, capabilities.surfaceCapabilities.minImageExtent.height,
							 capabilities.surfaceCapabilities.maxImageExtent.height);

  // clamp the prefered image count between the minimum and maximum supported by implementation
  image_count = std::clamp(image_count, capabilities.surfaceCapabilities.minImageCount,
						   capabilities.surfaceCapabilities.maxImageCount);

  auto swapchain_info = vk::SwapchainCreateInfoKHR {{},
													*m_surface,
													image_count,
													format.surfaceFormat.format,
													format.surfaceFormat.colorSpace,
													extent,
													1,
													image_usage,
													sharing_mode,
													queue_family_indices,
													transform,
													alpha,
													present_mode};

  auto swapchain = vk::raii::SwapchainKHR {m_device, swapchain_info};

  auto image_views = std::vector<vk::raii::ImageView> {};
  image_views.reserve(swapchain.getImages().size());
  auto image_view_info = vk::ImageViewCreateInfo({}, {}, swapchain::defaults::m_image_view_type,
												 format.surfaceFormat.format, {},
												 {swapchain::defaults::m_image_aspect, 0, 1, 0, 1});

  for (auto& image : swapchain.getImages())
  {
	image_view_info.image = image;
	image_views.emplace_back(m_device, image_view_info);
  }

  return {capabilities, format, present_mode, extent, {std::move(swapchain)}, std::move(image_views)};
}*/
/*
auto lh::renderer::create_physical_device() -> physical_device
{
  // enumerate all vulkan capable physical devices
  auto physical_devices = m_instance.enumeratePhysicalDevices();

  // assert that there are any vulkan capable devices
  if (physical_devices.empty())
	output::fatal() << "this system does not support any vulkan capable devices";

  // sort them according to their performance score
  std::ranges::sort(physical_devices,
					[](const auto& x, const auto& y) {
					  return physical_device {x}.get_performance_score() < physical_device {y}.get_performance_score();
					});

  // assert that the device with the highest score is above the minimum score threshold
  auto strongest_device = physical_device {physical_devices.front()};
  if (strongest_device.get_performance_score() < physical_device::defaults::m_minimum_accepted_score)
	output::fatal() << "this system does not have any suitable vulkan devices";

  return strongest_device;
}
*/
/*
auto lh::renderer::create_command_buffer() -> vk::raii::CommandBuffer
{
	// allocate a command_buffer from the command_pool
	auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo {*m_command_pool,
																	   vk::CommandBufferLevel::ePrimary,
																	   1};
	return std::move(vk::raii::CommandBuffers(m_device, command_buffer_allocate_info).front());
}
*/
auto lh::renderer::create_surface(const window& window) -> vk::raii::SurfaceKHR
{
	auto surface = VkSurfaceKHR {};
	glfwCreateWindowSurface(static_cast<VkInstance>(**m_instance), window.vkfw_window(), nullptr, &surface);

	return {m_instance, surface};
}

auto lh::renderer::create_extent(const window& window) -> vk::Extent2D
{
	return {window.resolution().width, window.resolution().height};
}
/*
auto lh::renderer::create_queue_families() -> queue_families
{
	const auto queue_family_properties = vk::raii::PhysicalDevice(m_physical_device).getQueueFamilyProperties2();
	auto queue_families = renderer::queue_families {};
	auto counter = queue_families::index_t {};

	for (const auto& queue_family_property : queue_family_properties)
	{
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
			queue_families.m_graphics = counter;
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute)
			queue_families.m_compute = counter;
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eTransfer)
			queue_families.m_transfer = counter;

		counter++;
	}

	for (queue_families::index_t i {}; i < std::numeric_limits<queue_families::index_t>::max(); i++)
		if (vk::raii::PhysicalDevice(m_physical_device).getSurfaceSupportKHR(0, *m_surface))
		{
			queue_families.m_present = i;
			break;
		}

	return queue_families;
}*/
/*
auto lh::renderer::create_swapchain(const window& window) -> vk::raii::SwapchainKHR
{
  // get the supported surface formats
  auto formats = vk::raii::PhysicalDevice(m_physical_device).getSurfaceFormatsKHR(*m_surface);

  if (formats.empty())
	output::fatal() << "this system does not support the required vulkan components";

  auto format = (formats.at(0).format == vk::Format::eUndefined) ? vk::Format::eB8G8R8A8Unorm : formats.at(0).format;

  // get the supported surface capabilities
  auto surface_capabilities = vk::raii::PhysicalDevice(m_physical_device).getSurfaceCapabilitiesKHR(*m_surface);
  auto swapchain_extent = vk::Extent2D {};

  // if the surface size is undefined, the size is set to the size of the images requested
  if (surface_capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
  {
	swapchain_extent.width = glm::clamp(window.get_resolution().width, surface_capabilities.minImageExtent.width,
										surface_capabilities.maxImageExtent.width);
	swapchain_extent.height = glm::clamp(window.get_resolution().height, surface_capabilities.minImageExtent.height,
										 surface_capabilities.maxImageExtent.height);
  }
  // if the surface size is defined, the swap chain size must match
  else
  {
	swapchain_extent = surface_capabilities.currentExtent;
  }

  // the FIFO present mode is guaranteed by the spec to be supported
  auto swapchain_present_mode = vk::PresentModeKHR::eFifo;

  auto surface_transform = (surface_capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
							 ? vk::SurfaceTransformFlagBitsKHR::eIdentity
							 : surface_capabilities.currentTransform;

  auto composite_alpha = (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
						   ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
						 : (surface_capabilities.supportedCompositeAlpha &
							vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
						   ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
						 : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
						   ? vk::CompositeAlphaFlagBitsKHR::eInherit
						   : vk::CompositeAlphaFlagBitsKHR::eOpaque;

  auto swapchain_info = vk::SwapchainCreateInfoKHR {};
  swapchain_info.flags = vk::SwapchainCreateFlagsKHR {};
  swapchain_info.surface = *m_surface;
  swapchain_info.minImageCount = surface_capabilities.minImageCount;
  swapchain_info.imageFormat = format;
  swapchain_info.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
  swapchain_info.imageExtent = swapchain_extent;
  swapchain_info.imageArrayLayers = 1;
  swapchain_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
  swapchain_info.imageSharingMode = vk::SharingMode::eExclusive;
  swapchain_info.queueFamilyIndexCount = {};
  swapchain_info.preTransform = surface_transform;
  swapchain_info.compositeAlpha = composite_alpha;
  swapchain_info.presentMode = swapchain_present_mode;
  swapchain_info.clipped = true;
  swapchain_info.oldSwapchain = nullptr;

  auto swapchain = vk::raii::SwapchainKHR {m_device, swapchain_info};

  return swapchain;
}
*/
/*
auto lh::renderer::create_swapchain_data(const window& window) -> vk::raii::su::SwapChainData
{
	 return {m_physical_device,
			 m_device,
			 m_surface,
			 {window.get_resolution().first, window.get_resolution().second},
			 vk::ImageUsageFlagBits::eColorAttachment |
	 vk::ImageUsageFlagBits::eTransferSrc,
			 {},
			 m_graphics_family_queue_indices.first,
			 m_graphics_family_queue_indices.second};
  return {m_physical_device,
		  m_device,
		  m_surface,
		  m_extent,
		  vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
		  {},
		  m_queue_families.m_graphics,
		  m_queue_families.m_present};
}
*/
/*
auto lh::renderer::create_image_views() -> std::vector<vk::raii::ImageView>
{
  auto swapchain_images = m_swapchain.m_swapchain.getImages(); // m_swapchain_data.images;

  auto image_views = std::vector<vk::raii::ImageView> {};
  image_views.reserve(swapchain_images.size());

  auto image_view_info = vk::ImageViewCreateInfo {};
  image_view_info.viewType = vk::ImageViewType::e2D;
  image_view_info.format = vk::Format::eB8G8R8A8Unorm;
  image_view_info.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

  for (auto image : swapchain_images)
  {
	image_view_info.image = image;
	image_views.push_back({m_device, image_view_info});
  }

  return image_views;
}
*/
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
	/*
	// create a DescriptorSetLayout
	auto descriptor_set_layout_binding =
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);

	auto descriptor_set_info = vk::DescriptorSetLayoutCreateInfo({}, descriptor_set_layout_binding);
	auto descriptor_set_layout = vk::raii::DescriptorSetLayout(m_device, descriptor_set_info);

	// create a PipelineLayout using that DescriptorSetLayout
	auto pipeline_layout_info = vk::PipelineLayoutCreateInfo({}, *descriptor_set_layout);
	return {m_device, pipeline_layout_info};*/

	return {m_device, {{}, **m_descriptor_set_layout}};
}

auto lh::renderer::create_format() -> vk::Format
{
	return vk::su::pickSurfaceFormat(vk::raii::PhysicalDevice(m_physical_device).getSurfaceFormatsKHR(**m_surface))
		.format;
}

auto lh::renderer::create_descriptor_set() -> vk::raii::DescriptorSet
{
	/*
	// create a descriptor pool
	auto pool_size = vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1);
	auto descriptor_pool_info =
		vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1, pool_size);

	auto descriptor_pool = vk::raii::DescriptorPool(m_device, descriptor_pool_info);
	auto descriptor_set_layout = vk::raii::su::makeDescriptorSetLayout(
		m_device, {{vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}});
	// allocate a descriptor set
	auto descriptor_set_info = vk::DescriptorSetAllocateInfo(*descriptor_pool, *descriptor_set_layout);

	return std::move(vk::raii::DescriptorSets(m_device, descriptor_set_info).front());
	*/

	vk::raii::DescriptorSet descriptorSet = std::move(
		vk::raii::DescriptorSets(m_device, {*m_descriptor_pool, **m_descriptor_set_layout}).front());
	vk::raii::su::updateDescriptorSets(m_device,
									   descriptorSet,
									   {{vk::DescriptorType::eUniformBuffer, m_uniform_buffer, VK_WHOLE_SIZE, nullptr}},
									   {});

	return descriptorSet;
}

auto lh::renderer::create_pipeline_cache() -> vk::raii::PipelineCache
{
	return {m_device, vk::PipelineCacheCreateInfo {}};
}

auto lh::renderer::create_pipeline() -> vk::raii::Pipeline
{
	return vk::raii::su::makeGraphicsPipeline(m_device,
											  m_pipeline_cache,
											  m_vertex,
											  nullptr,
											  m_fragment,
											  nullptr,
											  vk::su::checked_cast<uint32_t>(sizeof(coloredCubeData[0])),
											  {{vk::Format::eR32G32B32A32Sfloat, 0},
											   {vk::Format::eR32G32B32A32Sfloat, 16}},
											  vk::FrontFace::eClockwise,
											  true,
											  m_pipeline_layout,
											  m_renderpass);
}
/*
auto lh::renderer::create_render_pass(const window& window) -> vk::raii::RenderPass
{
	 auto color_format =
	 vk::su::pickSurfaceFormat(m_physical_device.getSurfaceFormatsKHR(*m_surface)).format; auto
	 depth_format = vk::Format::eD16Unorm;

	 auto attachment_descriptions = std::array<vk::AttachmentDescription, 2> {};

	 attachment_descriptions[0] = vk::AttachmentDescription(
		 {}, color_format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
	 vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
	 vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);

	 attachment_descriptions[1] = vk::AttachmentDescription(
		 {}, depth_format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
	 vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
	 vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	 auto color_reference = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
	 auto depth_reference = vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	 auto subpass_description =
		 vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, {}, color_reference, {},
	 &depth_reference);

	 auto render_pass_info = vk::RenderPassCreateInfo({}, attachment_descriptions, subpass_description);

	 return {m_device, render_pass_info};
	return vk::raii::su::makeRenderPass(m_device,
										m_swapchain.m_surface_format.surfaceFormat.format,
										m_depth_buffer.m_format);
}
*/
/*
auto lh::renderer::create_shader_module(const vk::ShaderStageFlagBits& stage) -> vk::raii::ShaderModule
{
	   glslang::InitializeProcess();

	   auto vertex_shader_spirv = std::vector<unsigned int> {};
	   auto shader_code = (stage == vk::ShaderStageFlagBits::eVertex) ? vertexShaderText_PC_C
	   : fragmentShaderText_C_C;

	   vk::su::GLSLtoSPV(stage, shader_code, vertex_shader_spirv);

	   auto shader_module_info = vk::ShaderModuleCreateInfo({}, vertex_shader_spirv);
	   auto shader_module = vk::raii::ShaderModule {m_device, shader_module_info};

	   glslang::FinalizeProcess();

	   return shader_module;

	glslang::InitializeProcess();

	auto& shader_code = (stage == vk::ShaderStageFlagBits::eVertex) ? vertexShaderText_PC_C : fragmentShaderText_C_C;
	auto module = vk::raii::su::makeShaderModule(m_device, stage, shader_code);

	glslang::FinalizeProcess();

	return module;
}
*/
/*
auto lh::renderer::create_framebuffers(const window& window) -> std::vector<vk::raii::Framebuffer>
{ /*
	 auto attachments = std::array<vk::ImageView, 2> {};
	 attachments[1] = *m_depth_buffer;

	 auto framebuffers = std::vector<vk::raii::Framebuffer> {};
	 framebuffers.reserve(m_swapchain.getImages().size());

	 for (auto const& view : m_image_views)
	 {
		 attachments[0] = *view;
		 auto framebuffer_info = vk::FramebufferCreateInfo(
			 {}, *m_render_pass, attachments, window.get_resolution().first,
	 window.get_resolution().second, 1);

		 framebuffers.push_back({m_device, framebuffer_info});
	 }

	 return framebuffers;

	return vk::raii::su::makeFramebuffers(
		m_device, *m_renderpass, m_swapchain.m_image_views, &m_depth_buffer.m_view, m_extent);
}
*/
auto lh::renderer::create_vertex_buffer() -> vk::raii::su::BufferData
{ /*
	 // create a vertex buffer for some vertex and color data
	 auto buffer_info = vk::BufferCreateInfo({}, sizeof(coloredCubeData),
	 vk::BufferUsageFlagBits::eVertexBuffer); auto vertex_buffer = vk::raii::Buffer(m_device, buffer_info);

	 auto graphics_queue = vk::raii::Queue(m_device, m_graphics_and_present_queue_indices.first, 0);

	 // allocate device memory for that buffer
	 auto memory_requirements = vertex_buffer.getMemoryRequirements();
	 auto memory_type_index =
		 vk::su::findMemoryType(m_physical_device.getMemoryProperties(),
	 memory_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible |
	 vk::MemoryPropertyFlagBits::eHostCoherent);

	 auto memory_info = vk::MemoryAllocateInfo(memory_requirements.size, memory_type_index);
	 auto memory = vk::raii::DeviceMemory(m_device, memory_info);

	 // copy the vertex and color data into that device memory
	 auto data = static_cast<uint8_t*>(memory.mapMemory(0, memory_requirements.size));
	 memcpy(data, coloredCubeData, sizeof(coloredCubeData));
	 memory.unmapMemory();

	 // and bind the device memory to the vertex buffer
	 vertex_buffer.bindMemory(*memory, 0);
	 */
	/*
	auto image_acquired_semaphore = vk::raii::Semaphore(m_device, vk::SemaphoreCreateInfo());

	auto result = vk::Result {};
	auto image_index = uint32_t {};
	std::tie(result, image_index) =
		m_swapchain_data.swapChain.acquireNextImage(vk::su::FenceTimeout, *image_acquired_semaphore);

	auto clear_values = std::array<vk::ClearValue, 2> {vk::ClearColorValue(0.2f, 0.2f, 0.2f, 0.2f),
													 vk::ClearDepthStencilValue(1.0f, 0)};

	m_command_buffer.begin({});

	auto render_pass_begin_info =
		vk::RenderPassBeginInfo(*m_render_pass, *m_framebuffers[image_index],
								vk::Rect2D(vk::Offset2D(0, 0), m_surface_data.extent), clear_values);
	m_command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

	m_command_buffer.bindVertexBuffers(0, {*vertex_buffer}, {0});

	m_command_buffer.endRenderPass();
	m_command_buffer.end();
	vk::raii::su::submitAndWait(m_device, graphics_queue, m_command_buffer);*/

	vk::raii::su::BufferData vertexBufferData(m_physical_device,
											  m_device,
											  sizeof(coloredCubeData),
											  vk::BufferUsageFlagBits::eVertexBuffer);
	vk::raii::su::copyToDevice(vertexBufferData.deviceMemory,
							   coloredCubeData,
							   sizeof(coloredCubeData) / sizeof(coloredCubeData[0]));

	return vertexBufferData;
}

auto lh::renderer::create_descriptor_pool() -> vk::raii::DescriptorPool
{
	return vk::raii::su::makeDescriptorPool(m_device, {{vk::DescriptorType::eUniformBuffer, 1}});
}
/*
auto lh::renderer::create_buffer(const data_t& data, const vk::BufferUsageFlagBits& usage) -> vk::raii::Buffer
{
	auto buffer_info = vk::BufferCreateInfo({}, data.size(), usage);
	auto buffer = vk::raii::Buffer(m_device, buffer_info);

	auto memory_requirements = buffer.getMemoryRequirements();
	auto type_index = vk::su::findMemoryType(vk::raii::PhysicalDevice(m_physical_device).getMemoryProperties(),
											 memory_requirements.memoryTypeBits,
											 vk::MemoryPropertyFlagBits::eHostVisible |
												 vk::MemoryPropertyFlagBits::eHostCoherent);

	auto memory_info = vk::MemoryAllocateInfo(memory_requirements.size, type_index);
	auto buffer_memory = vk::raii::DeviceMemory(m_device, memory_info);

	auto memory = static_cast<uint8_t*>(buffer_memory.mapMemory(0, memory_requirements.size));
	memcpy(memory, data.data(), data.size());

	buffer_memory.unmapMemory();
	buffer.bindMemory(*buffer_memory, 0);
}*/
/*
auto lh::renderer::physical_device::basic_info() const -> lh::output::string_t
{
	const auto properties = m_object.getProperties2();
	const auto memory = memory::physical_device_memory(m_object);
	constexpr auto gigabyte = static_cast<double>(1_gb);

	auto info = output::string_t {"\n======== basic vulkan device information: ========"};
	info += "\n\tname: " + output::string_t {properties.properties.deviceName.data()};
	info += "\n\tapi version: " + lh::string::string_t(lh::version {properties.properties.apiVersion});
	info += "\n\tdriver version: " + lh::string::string_t(lh::version {properties.properties.driverVersion});
	info += "\n\ttotal memory: " + std::to_string(double(memory.m_device_total) / gigabyte) + " gygabites";
	info += "\n\tavailable memory: " + std::to_string(double(memory.m_device_available) / gigabyte) + " gygabites";
	info += "\n\tused memory: " + std::to_string(double(memory.m_device_used) / gigabyte) + " gigabytes";
	info += "\n\tfree memory: " + std::to_string(memory.m_device_used_percentage) + " %\n";

	return info;
}

auto lh::renderer::physical_device::advanced_info() const -> output::string_t
{
	const auto properties = m_object.getProperties2();

	auto info = output::string_t {"\n======== advanced vulkan device information: ========"};
	info += "\n\tmax bound descriptor sets: " + std::to_string(properties.properties.limits.maxBoundDescriptorSets);
	info += "\n\tmax push constant size: " + std::to_string(properties.properties.limits.maxPushConstantsSize) +
			" bytes\n";

	return info;
}
*/
/*
auto lh::renderer::physical_device::get_performance_score() const -> performance_score_t
{
  const auto properties = m_device.getProperties2();
  const auto features = m_device.getFeatures2();

  constexpr auto gigabyte = static_cast<double>(1_gb);
  const auto memory = memory::physical_device_memory(m_device);

  auto score = performance_score_t {};

  score += memory.m_device_total + memory.m_shared_total;
  score += properties.properties.limits.maxImageDimension2D;
  score += properties.properties.limits.maxFramebufferWidth * properties.properties.limits.maxFramebufferHeight;

  return score;
}
*/
// ============================================================================
auto lh::renderer::render() -> void
{
	// Get the index of the next available swapchain image:
	vk::raii::Semaphore imageAcquiredSemaphore(m_device, vk::SemaphoreCreateInfo());

	const auto& command_buffer = m_command_control.command_buffers().front();

	vk::Result result;
	uint32_t imageIndex;
	std::tie(result, imageIndex) = m_swapchain->acquireNextImage(vk::su::FenceTimeout, *imageAcquiredSemaphore);

	command_buffer.begin({});

	std::array<vk::ClearValue, 2> clearValues;
	clearValues[0].color = vk::ClearColorValue(0.2f, 0.2f, 0.2f, 0.2f);
	clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
	vk::RenderPassBeginInfo renderPassBeginInfo(**m_renderpass,
												**m_swapchain.m_framebuffers[imageIndex],
												vk::Rect2D(vk::Offset2D(0, 0), m_surface.extent()),
												clearValues);
	// dynamic rendering
	const auto color_attachment = vk::RenderingAttachmentInfo {};
	const auto rendering_info = vk::RenderingInfo {{}, {{0, 0}, m_surface.extent()}, 1, 0, color_attachment};
	// dynamic rendering

	command_buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
	// command_buffer.beginRendering(rendering_info);

	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
	command_buffer.bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics, *m_pipeline_layout, 0, {*m_descriptor_set}, nullptr);

	command_buffer.bindVertexBuffers(0, {*m_vertex_buffer.buffer}, {0});
	command_buffer.setViewport(0,
							   vk::Viewport(0.0f,
											0.0f,
											static_cast<float>(m_surface.extent().width),
											static_cast<float>(m_surface.extent().height),
											0.0f,
											1.0f));
	command_buffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_surface.extent()));

	command_buffer.draw(12 * 3, 1, 0, 0);
	command_buffer.endRenderPass();
	// command_buffer.endRendering();
	command_buffer.end();

	vk::raii::Fence drawFence(m_device, vk::FenceCreateInfo());

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	vk::SubmitInfo submitInfo(*imageAcquiredSemaphore, waitDestinationStageMask, *command_buffer);
	m_queue.graphics().submit(submitInfo, *drawFence);

	while (vk::Result::eTimeout == m_device->waitForFences({*drawFence}, VK_TRUE, vk::su::FenceTimeout))
		;

	glm::mat4x4 mvpcMatrix = vk::su::createModelViewProjectionClipMatrix(m_surface.extent());
	mvpcMatrix = glm::rotate(mvpcMatrix, float(glm::sin(vkfw::getTime().value)), glm::vec3 {1.0f, 1.0f, 1.0f});

	// vk::raii::su::copyToDevice(m_uniform_buffer.memory(), mvpcMatrix);
	m_uniform_buffer.map_data(mvpcMatrix);
	vk::PresentInfoKHR presentInfoKHR(nullptr, **m_swapchain, imageIndex);
	result = m_queue.present().presentKHR(presentInfoKHR);
	switch (result)
	{
		case vk::Result::eSuccess: break;
		case vk::Result::eSuboptimalKHR:
			std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
			break;
		default: assert(false); // an unexpected result is returned !
	}
	// std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	m_device->waitIdle();
}

auto lh::renderer::dynamic_render() -> void
{
	// Get the index of the next available swapchain image:
	vk::raii::Semaphore imageAcquiredSemaphore(m_device, vk::SemaphoreCreateInfo());

	const auto& command_buffer = m_command_control.command_buffers().front();

	vk::Result result;
	uint32_t imageIndex;
	std::tie(result, imageIndex) = m_new_swapchain->acquireNextImage(vk::su::FenceTimeout, *imageAcquiredSemaphore);

	command_buffer.begin({});

	// dynamic rendering
	const auto color_attachment = vk::RenderingAttachmentInfo {*m_new_swapchain.views()[0],
															   vk::ImageLayout::eAttachmentOptimal};

	const auto depth_attachment = vk::RenderingAttachmentInfo {*m_dyn_rend_image.view(),
															   vk::ImageLayout::eDepthAttachmentOptimal};

	const auto formats = std::array {vk::Format::eR8G8B8A8Srgb, vk::Format::eD16Unorm};
	const auto pipeline_rendering_info = vk::PipelineRenderingCreateInfo {0, formats[0], formats[1]};
	const auto pipeline_stages =
		std::array {vk::PipelineShaderStageCreateInfo {{}, vk::ShaderStageFlagBits::eVertex, **m_vertex},
					vk::PipelineShaderStageCreateInfo {{}, vk::ShaderStageFlagBits::eFragment, **m_fragment}};

	auto graphics_pipeline_info = vk::GraphicsPipelineCreateInfo {{}, pipeline_stages};
	graphics_pipeline_info.pNext = &pipeline_rendering_info;

	const auto rendering_info =
		vk::RenderingInfo {{}, {{0, 0}, m_surface.extent()}, 1, 0, color_attachment, &depth_attachment};

	command_buffer.beginRendering(rendering_info);

	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
	command_buffer.bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics, *m_pipeline_layout, 0, {*m_descriptor_set}, nullptr);

	command_buffer.bindVertexBuffers(0, {*m_vertex_buffer.buffer}, {0});
	command_buffer.setViewport(0,
							   vk::Viewport(0.0f,
											0.0f,
											static_cast<float>(m_surface.extent().width),
											static_cast<float>(m_surface.extent().height),
											0.0f,
											1.0f));
	command_buffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_surface.extent()));

	command_buffer.draw(12 * 3, 1, 0, 0);

	command_buffer.endRendering();
	command_buffer.end();

	vk::raii::Fence drawFence(m_device, vk::FenceCreateInfo());

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	vk::SubmitInfo submitInfo(*imageAcquiredSemaphore, waitDestinationStageMask, *command_buffer);
	m_queue.graphics().submit(submitInfo, *drawFence);

	while (vk::Result::eTimeout == m_device->waitForFences({*drawFence}, VK_TRUE, vk::su::FenceTimeout))
		;

	glm::mat4x4 mvpcMatrix = vk::su::createModelViewProjectionClipMatrix(m_surface.extent());
	mvpcMatrix = glm::rotate(mvpcMatrix, float(glm::sin(vkfw::getTime().value)), glm::vec3 {1.0f, 1.0f, 1.0f});

	vk::raii::su::copyToDevice(m_uniform_buffer.memory(), mvpcMatrix);

	vk::PresentInfoKHR presentInfoKHR(nullptr, **m_new_swapchain, imageIndex);
	result = m_queue.present().presentKHR(presentInfoKHR);
	switch (result)
	{
		case vk::Result::eSuccess: break;
		case vk::Result::eSuboptimalKHR:
			std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
			break;
		default: assert(false); // an unexpected result is returned !
	}
	// std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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

VKAPI_ATTR auto VKAPI_CALL
lh::renderer::validation_module::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
												VkDebugUtilsMessageTypeFlagsEXT message_type,
												const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
												void* user_data) -> VkBool32
{
	auto message = output::string_t {};

	message += vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(message_severity)) + ": " +
			   vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(message_type)) + ":\n";
	message += std::string("\t") + "messageIDName   = <" + callback_data->pMessageIdName + ">\n";
	message += std::string("\t") + "messageIdNumber = " + std::to_string(callback_data->messageIdNumber) + "\n";
	message += std::string("\t") + "message         = <" + callback_data->pMessage + ">\n";

	if (callback_data->queueLabelCount > 0)
	{
		message += std::string("\t") + "Queue Labels:\n";

		for (uint32_t i = 0; i < callback_data->queueLabelCount; i++)
			message += std::string("\t\t") + "labelName = <" + callback_data->pQueueLabels[i].pLabelName + ">\n";
	}
	if (callback_data->cmdBufLabelCount > 0)
	{
		message += std::string("\t") + "CommandBuffer Labels:\n";

		for (uint32_t i = 0; i < callback_data->cmdBufLabelCount; i++)
			message += std::string("\t\t") + "labelName = <" + callback_data->pCmdBufLabels[i].pLabelName + ">\n";
	}
	if (callback_data->objectCount > 0)
	{
		for (uint32_t i = 0; i < callback_data->objectCount; i++)
		{
			message += std::string("\t") + "Object " + std::to_string(i) + "\n";
			message += std::string("\t\t") + "objectType = " +
					   vk::to_string(static_cast<vk::ObjectType>(callback_data->pObjects[i].objectType)) + "\n";
			message += std::string("\t\t") +
					   "objectHandle = " + std::to_string(callback_data->pObjects[i].objectHandle) + "\n";
			if (callback_data->pObjects[i].pObjectName)
				message += std::string("\t\t") + "objectName = <" + callback_data->pObjects[i].pObjectName + ">\n";
		}
	}

	switch (message_severity)
	{
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			output::log() << message;
			break;
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			output::warning() << message;
			break;
		case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			output::error() << message;
			break;
		default: break;
	}

	return false;
}
/*
auto lh::renderer::physical_device::required_extensions() const -> vulkan::vk_string_t
{
	return m_defaults.m_required_extensions;
}

auto lh::renderer::physical_device::supported_extensions() const -> vulkan::vk_extensions_t
{
	return m_object.enumerateDeviceExtensionProperties();
}

auto lh::renderer::physical_device::assert_required_extensions() -> bool
{
	const auto supported_extensions = physical_device::supported_extensions();
	const auto required_extensions = physical_device::required_extensions();

	for (const auto& required : required_extensions)
	{
		if (std::find_if(supported_extensions.begin(), supported_extensions.end(), [&required](const auto&
supported) { return strcmp(required, supported.extensionName) == 0;
			}) == supported_extensions.end())
		{
			output::error() << "this system does not support the required vulkan physical extension: " +
								   std::string {required};
			return false;
		}
	}

	return true;
}

auto lh::renderer::physical_device::get_performance_score(const vk::raii::PhysicalDevice& device) ->
performance_score_t
{
	const auto properties = device.getProperties2();
	const auto features = device.getFeatures2();

	constexpr auto gigabyte = static_cast<double>(1_gb);
	const auto memory = memory::physical_device_memory(device);

	auto score = performance_score_t {};

	score += memory.m_device_total + memory.m_shared_total;
	score += properties.properties.limits.maxImageDimension2D;
	score += performance_score_t {properties.properties.limits.maxFramebufferWidth *
								  properties.properties.limits.maxFramebufferHeight};

	return score;
}
*/
lh::renderer::memory_allocator::memory_allocator(const vulkan::instance& instance,
												 const vulkan::physical_device& physical_device,
												 const vulkan::logical_device& device)
{
	auto allocator_info = vma::AllocatorCreateInfo {{},
													**physical_device,
													**device,
													0,
													nullptr,
													nullptr,
													nullptr,
													nullptr,
													**instance,
													instance.version(),
													nullptr};

	auto result = vma::createAllocator(&allocator_info, &m_allocator);

	if (result != vk::Result::eSuccess)
		output::fatal() << "unable to initialize vulkan memory allocator";
}

lh::renderer::memory_allocator::~memory_allocator()
{
	m_allocator.destroy();
}

lh::renderer::memory_allocator::operator vma::Allocator&()
{
	return m_allocator;
}
/*
lh::renderer::physical_device::physical_device(const vk::raii::Instance& instance, const create_info& create_info)
{
	// enumerate all vulkan capable physical devices
	auto physical_devices = instance.enumeratePhysicalDevices();

	// assert that there are any vulkan capable devices
	if (physical_devices.empty())
		output::fatal() << "this system does not support any vulkan capable devices";

	// sort them according to their performance score
	std::ranges::sort(physical_devices,
					  [](const auto& x, const auto& y) { return get_performance_score(x) < get_performance_score(y);
});

	// assert that the device with the highest score is above the minimum score threshold
	auto& strongest_device = physical_devices.front();
	if (get_performance_score(strongest_device) < create_info.m_minimum_accepted_score)
		output::fatal() << "this system does not have any suitable vulkan devices";

	m_object = std::move(strongest_device);
}
*/
lh::renderer::swapchain::swapchain(const vulkan::physical_device& physical_device,
								   const vulkan::logical_device& device,
								   const vulkan::surface& surface,
								   const vulkan::queue_families& queue_families,
								   const vulkan::memory_allocator& memory_allocator,
								   const renderpass& renderpass,
								   const create_info& create_info)

	: m_surface_capabilities {(*physical_device).getSurfaceCapabilities2KHR(**surface)},
	  m_surface_format {},
	  m_present_mode {},
	  m_image_views {},
	  m_depth_buffer(physical_device, device, memory_allocator, surface, depth_buffer::m_defaults) /*,
	   m_framebuffers {}*/
{
	const auto& vk_physical_device = *physical_device;
	const auto capabilities = vk_physical_device.getSurfaceCapabilities2KHR(**surface);
	const auto formats = vk_physical_device.getSurfaceFormats2KHR(**surface);
	const auto present_modes = vk_physical_device.getSurfacePresentModesKHR(**surface);

	m_surface_format = create_info.m_format;
	m_present_mode = create_info.m_present_mode;
	auto image_count = create_info.m_image_count;
	auto image_usage = create_info.m_image_usage;
	auto sharing_mode = create_info.m_sharing_mode;
	auto transform = create_info.m_transform;
	auto alpha = create_info.m_alpha;

	auto queue_family_indices = {queue_families.graphics_index(), queue_families.present_index()};

	// assert that the system supports any formats and present modes
	if (formats.empty() || present_modes.empty())
		output::fatal() << "this system does not meet the minimal vulkan requirements";

	// attempt to acquire the prefered surface format, if unavailable, take the first one that is
	if (!std::ranges::contains(formats, create_info.m_format))
	{
		output::warning() << "this system does not support the prefered vulkan surface format";
		m_surface_format = formats.front();
	}

	// attempt to acquire the prefered present mode, if unavailable, default to FIFO
	if (!std::ranges::contains(present_modes, create_info.m_present_mode))
	{
		output::warning() << "this system does not support the prefered vulkan present mode";
		m_present_mode = vk::PresentModeKHR::eFifo;
	}

	// assert that the extent is within limits supported by the implementation
	if (surface.extent().width != std::clamp(surface.extent().width,
											 capabilities.surfaceCapabilities.minImageExtent.width,
											 capabilities.surfaceCapabilities.maxImageExtent.width))
		output::fatal() << "this system does not support windows of this width: " << surface.extent().width;

	if (surface.extent().height != std::clamp(surface.extent().height,
											  capabilities.surfaceCapabilities.minImageExtent.height,
											  capabilities.surfaceCapabilities.maxImageExtent.height))
		output::fatal() << "this system does not support windows of this height: " << surface.extent().height;

	// clamp the prefered image count between the minimum and maximum supported by implementation
	image_count = std::clamp(image_count,
							 capabilities.surfaceCapabilities.minImageCount,
							 capabilities.surfaceCapabilities.maxImageCount);

	auto swapchain_info = vk::SwapchainCreateInfoKHR {{},
													  **surface,
													  image_count,
													  m_surface_format.surfaceFormat.format,
													  m_surface_format.surfaceFormat.colorSpace,
													  surface.extent(),
													  1,
													  image_usage,
													  sharing_mode,
													  queue_family_indices,
													  transform,
													  alpha,
													  m_present_mode};

	m_object = {*device, swapchain_info};

	m_image_views.reserve(m_object.getImages().size());
	m_framebuffers.reserve(m_object.getImages().size());
	auto image_view_info = vk::ImageViewCreateInfo({},
												   {},
												   create_info.m_image_view_type,
												   m_surface_format.surfaceFormat.format,
												   {},
												   {create_info.m_image_aspect, 0, 1, 0, 1});

	for (auto& image : m_object.getImages())
	{
		image_view_info.image = image;
		m_image_views.emplace_back(*device, image_view_info);
		m_framebuffers.emplace_back(device, renderpass, m_image_views.back(), m_depth_buffer.m_view, surface);
	}
}

lh::renderer::logical_device::logical_device(const vulkan::physical_device& physical_device,
											 const std::vector<vk::DeviceQueueCreateInfo>& queues,
											 const vulkan::vk_string_t& extensions,
											 const create_info& create_info)
{
	const auto& suported_extensions = physical_device.extensions().supported_extensions();

	if (!renderer::assert_required_components(suported_extensions, extensions))
		output::fatal() << "this system does not support the required vulkan components";

	// const auto device_queue_info = vk::DeviceQueueCreateInfo {{}, m_queue_families.m_graphics, 1,
	// &queue_priority};
	auto device_info = vk::DeviceCreateInfo {{}, queues, {}, extensions, &create_info.m_features.features};

	m_object = {*physical_device, device_info};
}

lh::renderer::image::image(const vulkan::physical_device& physical_device,
						   const vulkan::logical_device& device,
						   const vulkan::memory_allocator& allocator,
						   const vulkan::surface& surface,
						   const create_info& create_info)
	: m_format(create_info.m_format), m_view {nullptr}, m_memory {nullptr}
{
	const auto image_info = vk::ImageCreateInfo {create_info.m_image_create_flags,
												 create_info.m_image_type,
												 create_info.m_format,
												 vk::Extent3D(surface.extent(), 1),
												 1,
												 1,
												 create_info.m_image_sample_count,
												 create_info.m_image_tiling,
												 create_info.m_image_usage | vk::ImageUsageFlagBits::eSampled,
												 create_info.m_image_sharing_mode,
												 {},
												 create_info.m_image_layout};

	const auto allocation_create_info = vma::AllocationCreateInfo {{}, vma::MemoryUsage::eAuto};
	auto allocation_info = vma::AllocationInfo {};

	auto [image, allocation] = allocator->createImage(image_info, allocation_create_info, allocation_info);

	m_object = {*device, image};

	const auto view_info = vk::ImageViewCreateInfo {
		{}, *m_object, create_info.m_view_type, create_info.m_format, {}, {create_info.m_image_aspect, 0, 1, 0, 1}};

	m_view = {*device, view_info};
	m_memory = {*device, allocation_info.deviceMemory};
}

lh::renderer::renderpass::renderpass(const vulkan::physical_device& physical_device,
									 const vulkan::logical_device& device,
									 const vulkan::surface& surface,
									 const create_info& create_info)
{
	const auto formats = physical_device->getSurfaceFormatsKHR(**surface);

	auto attachments = std::vector<vk::AttachmentDescription> {create_info.m_color_attachment,
															   create_info.m_depth_attachment};

	// attempt to acquire the prefered surface format, if unavailable, take the first one that is
	if (!std::ranges::contains(formats, create_info.m_color_attachment.format))
	{
		output::warning() << "this system does not support the prefered vulkan surface format";
		attachments.front().format = formats.front().format;
	}

	const auto subpass_description = vk::SubpassDescription {vk::SubpassDescriptionFlags(),
															 create_info.m_bind_point,
															 {},
															 create_info.m_color_attachment_reference,
															 {},
															 &create_info.m_depth_attachment_reference};

	const auto render_pass_info = vk::RenderPassCreateInfo {{}, attachments, subpass_description};

	m_object = {*device, render_pass_info};
}

lh::renderer::buffer::buffer(const vulkan::physical_device& physical_device,
							 const vulkan::logical_device& device,
							 const vma::Allocator& allocator,
							 const vk::DeviceSize& size,
							 const create_info& create_info)
	: m_memory {nullptr}
{
	const auto buffer_info = vk::BufferCreateInfo({}, size, create_info.m_usage);
	const auto allocation_create_info = vma::AllocationCreateInfo {{},
																   vma::MemoryUsage::eAuto,
																   create_info.m_properties};
	auto allocation_info = vma::AllocationInfo {};

	auto [buffer, allocation] = allocator.createBuffer(buffer_info, allocation_create_info, allocation_info);

	m_object = {*device, buffer};
	m_memory = {*device, allocation_info.deviceMemory};
}

lh::renderer::command_control::command_control(const vulkan::logical_device& device,
											   const vulkan::queue_families& queue_families,
											   const create_info& create_info)
	: m_buffers {nullptr}
{
	m_object = {*device, {create_info.m_pool_flags, queue_families.graphics_index()}};

	auto command_buffers_info = vk::CommandBufferAllocateInfo {*m_object,
															   create_info.m_buffer_level,
															   create_info.m_num_buffers};

	m_buffers = {*device, command_buffers_info};
}

lh::renderer::framebuffer::framebuffer(const vulkan::logical_device& device,
									   const renderpass& renderpass,
									   const image& image,
									   const depth_buffer& depth_buffer,
									   const vulkan::surface& surface,
									   const create_info& create_info)
	: framebuffer(device, renderpass, image.m_view, depth_buffer.m_view, surface, create_info)
{}

lh::renderer::framebuffer::framebuffer(const vulkan::logical_device& device,
									   const renderpass& renderpass,
									   const vk::raii::ImageView& image,
									   const vk::raii::ImageView& depth_buffer,
									   const vulkan::surface& surface,
									   const create_info& create_info)
{
	const auto views = std::array {*image, *depth_buffer};

	const auto framebuffer_info = vk::FramebufferCreateInfo {
		{}, **renderpass, 2, views.data(), surface.extent().width, surface.extent().height, 1};

	m_object = {*device, framebuffer_info};
}

lh::renderer::queue_families::queue_families(const vulkan::physical_device& physical_device,
											 const vk::raii::SurfaceKHR& surface,
											 const create_info& create_info)
	: m_graphics {}, m_present {}, m_compute {}, m_transfer {}
{
	const auto queue_family_properties = physical_device->getQueueFamilyProperties2();
	auto counter = queue_families::index_t {};

	for (const auto& queue_family_property : queue_family_properties)
	{
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
			m_graphics = counter;
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute)
			m_compute = counter;
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eTransfer)
			m_transfer = counter;

		counter++;
	}

	for (queue_families::index_t i {}; i < std::numeric_limits<queue_families::index_t>::max(); i++)
		if (physical_device->getSurfaceSupportKHR(0, *surface))
		{
			m_present = i;
			break;
		}
}

lh::renderer::descriptor_set_layout::descriptor_set_layout(const vulkan::logical_device& device,
														   const std::vector<binding>& bindings,
														   const create_info& create_info)
{
	auto layout_bindings = std::vector<vk::DescriptorSetLayoutBinding> {bindings.size()};

	for (const auto& binding : bindings)
		layout_bindings.emplace_back(binding.m_location, binding.m_type, binding.m_count, create_info.m_access);

	const auto layout_info = vk::DescriptorSetLayoutCreateInfo(create_info.m_flags, layout_bindings);

	m_object = {*device, layout_info};
}
