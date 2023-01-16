#include "renderer.hpp"

lh::renderer::renderer(const window& window, const engine_version& engine_version, const vulkan_version& vulkan_version,
                       bool use_validation_module)
    : m_version {vulkan_version},
      m_context {create_context()},
      m_instance {create_instance(window, engine_version, vulkan_version, use_validation_module)},
      m_validation_module {use_validation_module ? validation_module {m_instance}
                                                 : decltype(m_validation_module) {std::nullopt}},
      m_physical_device {create_physical_device()},
      // m_surface_data {create_surface_data(window)},
      m_extent {create_extent(window)},
      m_surface {create_surface(window)},
      m_graphics_and_present_queue_indices {create_graphics_family_queue_indices()},
      m_physical_extensions {m_physical_device},
      m_device {create_device()},
      m_command_pool {create_command_pool()},
      m_graphics_queue {create_graphics_queue()},
      m_present_queue {create_present_queue()},
      m_command_buffer {create_command_buffer()},
      // m_swapchain {create_swapchain(window)},
      m_swapchain_data {create_swapchain_data(window)},
      // m_image_views {create_image_views()},
      // m_depth_buffer {create_depth_buffer(window)},
      m_depth_buffer_data {create_depth_buffer_data(window)},
      m_uniform_buffer {create_uniform_buffer()},
      m_descriptor_set_layout {create_descriptor_set_layout()},
      m_pipeline_layout {create_pipeline_layout()},
      m_format {create_format()},
      m_descriptor_set {create_descriptor_set()},
      m_render_pass {create_render_pass(window)},
      m_shader_modules {create_shader_module(vk::ShaderStageFlagBits::eVertex),
                        create_shader_module(vk::ShaderStageFlagBits::eFragment)},
      m_vertex_buffer {create_vertex_buffer()},
      m_framebuffers {create_framebuffers(window)},
      m_descriptor_pool {create_descriptor_pool()},
      m_pipeline_cache {create_pipeline_cache()},
      m_pipeline {create_pipeline()}
{
}

auto lh::renderer::logical_extension_module::assert_required_extensions() -> bool
{
    // make sure the implementation supports all required extensions
    auto required_extensions = logical_extension_module::required_extensions();
    auto supported_extensions = logical_extension_module::supported_extensions();

    for (const auto& required : required_extensions)
    {
        if (std::find_if(supported_extensions.begin(), supported_extensions.end(),
                         [&required](const auto& supported)
                         { return strcmp(required, supported.extensionName) == 0; }) == supported_extensions.end())
        {
            output::error() << "this system does not support the required vulkan logical extension: " +
                                   std::string {required};
            return false;
        }
    }

    return true;
}

auto lh::renderer::validation_module::assert_required_validation_layers() -> bool
{
    // make sure the implementation supports all required validation layers
    auto required_layers = validation_module::required_validation_layers();
    auto supported_layers = validation_module::supported_validation_layers();

    for (const auto& required : required_layers)
    {
        if (std::find_if(supported_layers.begin(), supported_layers.end(),
                         [&required](const auto& supported)
                         { return strcmp(required, supported.layerName) == 0; }) == supported_layers.end())
        {
            output::error() << "this system does not support the required vulkan validation layer: " + std::string {required};
            return false;
        }
    }

    return true;
}

auto lh::renderer::logical_extension_module::supported_extensions() -> std::vector<vk::ExtensionProperties>
{
    // find the number of supported extensions first
    auto num_extensions = uint32_t {0};
    vk::enumerateInstanceExtensionProperties(nullptr, &num_extensions, nullptr);

    auto extensions = std::vector<vk::ExtensionProperties>(num_extensions);
    vk::enumerateInstanceExtensionProperties(nullptr, &num_extensions, extensions.data());

    return extensions;
}

auto lh::renderer::validation_module::supported_validation_layers() -> std::vector<vk::LayerProperties>
{
    // find the number of supported layers first
    auto num_layers = uint32_t {0};
    vk::enumerateInstanceLayerProperties(&num_layers, nullptr);

    auto layers = std::vector<vk::LayerProperties>(num_layers);
    vk::enumerateInstanceLayerProperties(&num_layers, layers.data());

    return layers;
}

auto lh::renderer::logical_extension_module::required_extensions() -> vk_string
{
    // combine the extensions required by glfw with those specified in m_required_extensions
    auto num_extensions = uint32_t {0};
    auto glfw_extensions = vkfw::getRequiredInstanceExtensions(&num_extensions);

    auto combined_extensions = vk_string {glfw_extensions, glfw_extensions + num_extensions};
    combined_extensions.insert(combined_extensions.end(), m_required_extensions.begin(), m_required_extensions.end());

    return combined_extensions;
}

lh::renderer::validation_module::validation_module(vk::raii::Instance& instance)
    : m_debug_messenger {
          instance.createDebugUtilsMessengerEXT({{}, m_message_severity, m_message_type, &debug_callback})}
{
}

auto lh::renderer::validation_module::required_validation_layers() -> vk_string
{
    return m_required_validation_layers;
}

auto lh::renderer::create_context() -> vk::raii::Context
{
    return {};
}

auto lh::renderer::create_instance(const window& window, const engine_version& engine_version,
                                   const vulkan_version& vulkan_version, bool use_validation_module)
    -> vk::raii::Instance
{
    auto app_info = vk::ApplicationInfo {};
    auto instance_info = vk::InstanceCreateInfo {};
    auto validation_module = true;

    // configure application info
    app_info.pApplicationName = window.get_title().data();
    app_info.applicationVersion = engine_version;
    app_info.pEngineName = app_info.pApplicationName;
    app_info.engineVersion = app_info.applicationVersion;
    app_info.apiVersion =
        VK_MAKE_API_VERSION(0, vulkan_version.m_major, vulkan_version.m_minor, vulkan_version.m_patch);

    if (use_validation_module)
        validation_module = m_validation_module->assert_required_validation_layers();

    // make sure both extension and validation layers checks passed
    if (!(m_logical_extensions.assert_required_extensions() && validation_module))
    {
        output::fatal() << "this system does not support the required vulkan components";
    }

    auto required_extensions = m_logical_extensions.required_extensions();
    auto required_validation_layers =
        use_validation_module ? m_validation_module->required_validation_layers() : vk_string {nullptr};

    auto instance_debugger = vk::DebugUtilsMessengerCreateInfoEXT {{},
                                                                   m_validation_module->m_message_severity,
                                                                   m_validation_module->m_message_type,
                                                                   &m_validation_module->debug_callback};

    // configure instance info
    instance_info.pApplicationInfo = &app_info;
    instance_info.pNext = use_validation_module ? &instance_debugger : nullptr;
    instance_info.enabledExtensionCount = required_extensions.size();
    instance_info.ppEnabledExtensionNames = required_extensions.data();
    instance_info.enabledLayerCount = use_validation_module ? required_validation_layers.size() : 0;
    instance_info.ppEnabledLayerNames = use_validation_module ? required_validation_layers.data() : nullptr;

    return {m_context, instance_info};
}

auto lh::renderer::create_device() -> vk::raii::Device
{
    // vulkan features
    const auto features_1 = m_physical_device.getFeatures();
    const auto features_2 = m_physical_device.getFeatures2();

    // physical extensions
    const auto required_extensions = m_physical_extensions.required_extensions();

    if (!m_physical_extensions.assert_required_extensions())
        output::fatal() << "this system does not support the required vulkan components";

    const auto queue_priority = 0.0f;

    auto device_queue_info = vk::DeviceQueueCreateInfo {};
    device_queue_info.queueFamilyIndex = m_graphics_and_present_queue_indices.first;
    device_queue_info.queueCount = 1;
    device_queue_info.pQueuePriorities = &queue_priority;

    auto device_info = vk::DeviceCreateInfo {};
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &device_queue_info;
    device_info.pEnabledFeatures = &features_1;
    device_info.enabledExtensionCount = required_extensions.size();
    device_info.ppEnabledExtensionNames = required_extensions.data();

    return {m_physical_device, device_info};
}

auto lh::renderer::create_command_pool() -> vk::raii::CommandPool
{
    // create a command pool to allocate a command buffer from
    /*auto command_pool_info = vk::CommandPoolCreateInfo {};
    command_pool_info.queueFamilyIndex = m_graphics_family_queue_indices.first;
    */
    return {m_device, {vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_graphics_and_present_queue_indices.first}};
}

auto lh::renderer::create_graphics_queue() -> vk::raii::Queue
{
    return {m_device, m_graphics_and_present_queue_indices.first, 0};
}

auto lh::renderer::create_present_queue() -> vk::raii::Queue
{
    return {m_device, m_graphics_and_present_queue_indices.second, 0};
}

auto lh::renderer::create_physical_device() -> vk::raii::PhysicalDevice
{
    return vk::raii::PhysicalDevices(m_instance).front();
}

auto lh::renderer::create_command_buffer() -> vk::raii::CommandBuffer
{
    /*
    // allocate a command_buffer from the command_pool
    auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo {};
    command_buffer_allocate_info.commandPool = *m_command_pool;
    command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
    command_buffer_allocate_info.commandBufferCount = 1;

    return std::move(vk::raii::CommandBuffers(m_device, command_buffer_allocate_info).front());*/

    return vk::raii::su::makeCommandBuffer(m_device, m_command_pool);
}

auto lh::renderer::create_surface(const window& window) -> vk::raii::SurfaceKHR
{
    auto surface = VkSurfaceKHR {};
    glfwCreateWindowSurface(static_cast<VkInstance>(*m_instance), window.vkfw_window(), nullptr, &surface);

    return {m_instance, surface};
}

auto lh::renderer::create_extent(const window& window) -> vk::Extent2D
{
    return {window.get_resolution().width, window.get_resolution().height};
}

auto lh::renderer::create_surface_data(const window& window) -> vk::raii::su::SurfaceData
{
    auto surface = vk::raii::su::SurfaceData {
        m_instance, window.get_title().data(), {window.get_resolution().width, window.get_resolution().height}};

    return surface;
}

auto lh::renderer::create_swapchain(const window& window) -> vk::raii::SwapchainKHR
{
    // get the supported surface formats
    auto formats = m_physical_device.getSurfaceFormatsKHR(*m_surface);

    if (formats.empty())
        output::fatal() << "this system does not support the required vulkan components";

    auto format = (formats.at(0).format == vk::Format::eUndefined) ? vk::Format::eB8G8R8A8Unorm : formats.at(0).format;

    // get the supported surface capabilities
    auto surface_capabilities = m_physical_device.getSurfaceCapabilitiesKHR(*m_surface);
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

    auto composite_alpha =
        (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
            ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
        : (surface_capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
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

auto lh::renderer::create_graphics_family_queue_indices() -> std::pair<uint32_t, uint32_t>
{
    return vk::raii::su::findGraphicsAndPresentQueueFamilyIndex(m_physical_device, m_surface);
}

auto lh::renderer::create_swapchain_data(const window& window) -> vk::raii::su::SwapChainData
{ /*
     return {m_physical_device,
             m_device,
             m_surface,
             {window.get_resolution().first, window.get_resolution().second},
             vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
             {},
             m_graphics_family_queue_indices.first,
             m_graphics_family_queue_indices.second};*/

    return {m_physical_device,
            m_device,
            m_surface,
            m_extent,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
            {},
            m_graphics_and_present_queue_indices.first,
            m_graphics_and_present_queue_indices.second};
}

auto lh::renderer::create_image_views() -> std::vector<vk::raii::ImageView>
{
    auto swapchain_images = m_swapchain_data.images;

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

auto lh::renderer::create_depth_buffer(const window& window) -> vk::raii::ImageView
{
    const auto depth_format = vk::Format::eD16Unorm;
    auto format_properties = m_physical_device.getFormatProperties(depth_format);

    auto tiling_mode = vk::ImageTiling {};

    if (format_properties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
    {
        tiling_mode = vk::ImageTiling::eLinear;
    }
    else if (format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
    {
        tiling_mode = vk::ImageTiling::eOptimal;
    }
    else
    {
        output::fatal() << "DepthStencilAttachment is not supported for D16Unorm depth format";
    }

    auto image_info = vk::ImageCreateInfo {};
    image_info.imageType = vk::ImageType::e2D;
    image_info.format = depth_format;
    image_info.extent = vk::Extent3D {window.get_resolution().width, window.get_resolution().height, 1};
    image_info.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;

    auto image = vk::raii::Image {m_device, image_info};

    auto memory_properties = m_physical_device.getMemoryProperties();
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

    auto image_view_info = vk::ImageViewCreateInfo({}, *image, vk::ImageViewType::e2D, depth_format, {},
                                                   {vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});

    return {m_device, image_view_info};
}

auto lh::renderer::create_depth_buffer_data(const window&) -> vk::raii::su::DepthBufferData
{
    return {m_physical_device, m_device, vk::Format::eD16Unorm, m_extent};
}

auto lh::renderer::create_uniform_buffer() -> vk::raii::su::BufferData
{
    vk::raii::su::BufferData uniformBufferData(m_physical_device, m_device, sizeof(glm::mat4x4),
                                               vk::BufferUsageFlagBits::eUniformBuffer);
    glm::mat4x4 mvpcMatrix = vk::su::createModelViewProjectionClipMatrix(m_extent);
    vk::raii::su::copyToDevice(uniformBufferData.deviceMemory, mvpcMatrix);

    return uniformBufferData;
}

auto lh::renderer::create_descriptor_set_layout() -> vk::raii::DescriptorSetLayout
{
    return vk::raii::su::makeDescriptorSetLayout(
        m_device, {{vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}});
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

    return {m_device, {{}, *m_descriptor_set_layout}};
}

auto lh::renderer::create_format() -> vk::Format
{
    return vk::su::pickSurfaceFormat(m_physical_device.getSurfaceFormatsKHR(*m_surface)).format;
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

    vk::raii::DescriptorSet descriptorSet =
        std::move(vk::raii::DescriptorSets(m_device, {*m_descriptor_pool, *m_descriptor_set_layout}).front());
    vk::raii::su::updateDescriptorSets(
        m_device, descriptorSet,
        {{vk::DescriptorType::eUniformBuffer, m_uniform_buffer.buffer, VK_WHOLE_SIZE, nullptr}}, {});

    return descriptorSet;
}

auto lh::renderer::create_pipeline_cache() -> vk::raii::PipelineCache
{
    return {m_device, vk::PipelineCacheCreateInfo {}};
}

auto lh::renderer::create_pipeline() -> vk::raii::Pipeline
{
    return vk::raii::su::makeGraphicsPipeline(
        m_device, m_pipeline_cache, m_shader_modules[0], nullptr, m_shader_modules[1], nullptr,
        vk::su::checked_cast<uint32_t>(sizeof(coloredCubeData[0])),
        {{vk::Format::eR32G32B32A32Sfloat, 0}, {vk::Format::eR32G32B32A32Sfloat, 16}}, vk::FrontFace::eClockwise, true,
        m_pipeline_layout, m_render_pass);
}

auto lh::renderer::create_render_pass(const window& window) -> vk::raii::RenderPass
{ /*
     auto color_format = vk::su::pickSurfaceFormat(m_physical_device.getSurfaceFormatsKHR(*m_surface)).format;
     auto depth_format = vk::Format::eD16Unorm;

     auto attachment_descriptions = std::array<vk::AttachmentDescription, 2> {};

     attachment_descriptions[0] = vk::AttachmentDescription(
         {}, color_format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
         vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
         vk::ImageLayout::ePresentSrcKHR);

     attachment_descriptions[1] = vk::AttachmentDescription(
         {}, depth_format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
         vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
         vk::ImageLayout::eDepthStencilAttachmentOptimal);

     auto color_reference = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
     auto depth_reference = vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

     auto subpass_description =
         vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, {}, color_reference, {}, &depth_reference);

     auto render_pass_info = vk::RenderPassCreateInfo({}, attachment_descriptions, subpass_description);

     return {m_device, render_pass_info};*/

    return vk::raii::su::makeRenderPass(m_device, m_format, m_depth_buffer_data.format);
}

auto lh::renderer::create_shader_module(const vk::ShaderStageFlagBits& stage) -> vk::raii::ShaderModule
{ /*
     glslang::InitializeProcess();

     auto vertex_shader_spirv = std::vector<unsigned int> {};
     auto shader_code = (stage == vk::ShaderStageFlagBits::eVertex) ? vertexShaderText_PC_C : fragmentShaderText_C_C;

     vk::su::GLSLtoSPV(stage, shader_code, vertex_shader_spirv);

     auto shader_module_info = vk::ShaderModuleCreateInfo({}, vertex_shader_spirv);
     auto shader_module = vk::raii::ShaderModule {m_device, shader_module_info};

     glslang::FinalizeProcess();

     return shader_module;*/

    glslang::InitializeProcess();

    auto shader_code = (stage == vk::ShaderStageFlagBits::eVertex) ? vertexShaderText_PC_C : fragmentShaderText_C_C;
    auto module = vk::raii::su::makeShaderModule(m_device, stage, shader_code);

    glslang::FinalizeProcess();

    return module;
}

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
             {}, *m_render_pass, attachments, window.get_resolution().first, window.get_resolution().second, 1);

         framebuffers.push_back({m_device, framebuffer_info});
     }

     return framebuffers;*/

    return vk::raii::su::makeFramebuffers(m_device, m_render_pass, m_swapchain_data.imageViews,
                                          &m_depth_buffer_data.imageView, m_extent);
}

auto lh::renderer::create_vertex_buffer() -> vk::raii::su::BufferData
{ /*
     // create a vertex buffer for some vertex and color data
     auto buffer_info = vk::BufferCreateInfo({}, sizeof(coloredCubeData), vk::BufferUsageFlagBits::eVertexBuffer);
     auto vertex_buffer = vk::raii::Buffer(m_device, buffer_info);

     auto graphics_queue = vk::raii::Queue(m_device, m_graphics_and_present_queue_indices.first, 0);

     // allocate device memory for that buffer
     auto memory_requirements = vertex_buffer.getMemoryRequirements();
     auto memory_type_index =
         vk::su::findMemoryType(m_physical_device.getMemoryProperties(), memory_requirements.memoryTypeBits,
                                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

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

    vk::raii::su::BufferData vertexBufferData(m_physical_device, m_device, sizeof(coloredCubeData),
                                              vk::BufferUsageFlagBits::eVertexBuffer);
    vk::raii::su::copyToDevice(vertexBufferData.deviceMemory, coloredCubeData,
                               sizeof(coloredCubeData) / sizeof(coloredCubeData[0]));

    return vertexBufferData;
}

auto lh::renderer::create_descriptor_pool() -> vk::raii::DescriptorPool
{
    return vk::raii::su::makeDescriptorPool(m_device, {{vk::DescriptorType::eUniformBuffer, 1}});
}

auto lh::renderer::create_buffer(const data_t& data, const vk::BufferUsageFlagBits& usage) -> vk::raii::Buffer
{

    auto buffer_info = vk::BufferCreateInfo({}, data.size(), usage);
    auto buffer = vk::raii::Buffer(m_device, buffer_info);

    auto memory_requirements = buffer.getMemoryRequirements();
    auto type_index =
        vk::su::findMemoryType(m_physical_device.getMemoryProperties(), memory_requirements.memoryTypeBits,
                               vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    auto memory_info = vk::MemoryAllocateInfo(memory_requirements.size, type_index);
    auto buffer_memory = vk::raii::DeviceMemory(m_device, memory_info);

    auto memory = static_cast<uint8_t*>(buffer_memory.mapMemory(0, memory_requirements.size));
    memcpy(memory, data.data(), data.size());

    buffer_memory.unmapMemory();
    buffer.bindMemory(*buffer_memory, 0);
}

auto lh::renderer::render() -> void
{
    // Get the index of the next available swapchain image:
    vk::raii::Semaphore imageAcquiredSemaphore(m_device, vk::SemaphoreCreateInfo());

    vk::Result result;
    uint32_t imageIndex;
    std::tie(result, imageIndex) =
        m_swapchain_data.swapChain.acquireNextImage(vk::su::FenceTimeout, *imageAcquiredSemaphore);
    assert(result == vk::Result::eSuccess);
    assert(imageIndex < swapChainData.images.size());

    m_command_buffer.begin({});

    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color = vk::ClearColorValue(0.2f, 0.2f, 0.2f, 0.2f);
    clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
    vk::RenderPassBeginInfo renderPassBeginInfo(*m_render_pass, *m_framebuffers[imageIndex],
                                                vk::Rect2D(vk::Offset2D(0, 0), m_extent), clearValues);
    m_command_buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    m_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    m_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *m_pipeline_layout, 0, {*m_descriptor_set},
                                        nullptr);

    m_command_buffer.bindVertexBuffers(0, {*m_vertex_buffer.buffer}, {0});
    m_command_buffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_extent.width),
                                                 static_cast<float>(m_extent.height), 0.0f, 1.0f));
    m_command_buffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_extent));

    m_command_buffer.draw(12 * 3, 1, 0, 0);
    m_command_buffer.endRenderPass();
    m_command_buffer.end();

    vk::raii::Fence drawFence(m_device, vk::FenceCreateInfo());

    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo submitInfo(*imageAcquiredSemaphore, waitDestinationStageMask, *m_command_buffer);
    m_graphics_queue.submit(submitInfo, *drawFence);

    while (vk::Result::eTimeout == m_device.waitForFences({*drawFence}, VK_TRUE, vk::su::FenceTimeout))
        ;

    glm::mat4x4 mvpcMatrix = vk::su::createModelViewProjectionClipMatrix(m_extent);
    mvpcMatrix = glm::rotate(mvpcMatrix, float(vkfw::getTime().value), glm::vec3 {0.0f, 1.0f, 0.0f});

    vk::raii::su::copyToDevice(m_uniform_buffer.deviceMemory, mvpcMatrix);

    vk::PresentInfoKHR presentInfoKHR(nullptr, *m_swapchain_data.swapChain, imageIndex);
    result = m_present_queue.presentKHR(presentInfoKHR);
    switch (result)
    {
    case vk::Result::eSuccess:
        break;
    case vk::Result::eSuboptimalKHR:
        std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
        break;
    default:
        assert(false); // an unexpected result is returned !
    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    /* VULKAN_KEY_END */

    m_device.waitIdle();
}

VKAPI_ATTR auto VKAPI_CALL lh::renderer::validation_module::debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32
{
    std::string message;

    message += vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) + ": " +
               vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageType)) + ":\n";
    message += std::string("\t") + "messageIDName   = <" + pCallbackData->pMessageIdName + ">\n";
    message += std::string("\t") + "messageIdNumber = " + std::to_string(pCallbackData->messageIdNumber) + "\n";
    message += std::string("\t") + "message         = <" + pCallbackData->pMessage + ">\n";
    if (0 < pCallbackData->queueLabelCount)
    {
        message += std::string("\t") + "Queue Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
        {
            message += std::string("\t\t") + "labelName = <" + pCallbackData->pQueueLabels[i].pLabelName + ">\n";
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount)
    {
        message += std::string("\t") + "CommandBuffer Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
        {
            message += std::string("\t\t") + "labelName = <" + pCallbackData->pCmdBufLabels[i].pLabelName + ">\n";
        }
    }
    if (0 < pCallbackData->objectCount)
    {
        for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
        {
            message += std::string("\t") + "Object " + std::to_string(i) + "\n";
            message += std::string("\t\t") + "objectType   = " +
                       vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) + "\n";
            message += std::string("\t\t") +
                       "objectHandle = " + std::to_string(pCallbackData->pObjects[i].objectHandle) + "\n";
            if (pCallbackData->pObjects[i].pObjectName)
            {
                message += std::string("\t\t") + "objectName   = <" + pCallbackData->pObjects[i].pObjectName + ">\n";
            }
        }
    }

    std::cout << message;

    return false;
}

auto lh::renderer::physical_extension_module::required_extensions() -> vk_string
{
    return m_required_extensions;
}

auto lh::renderer::physical_extension_module::supported_extensions() -> std::vector<vk::ExtensionProperties>
{
    return m_device.enumerateDeviceExtensionProperties();
}

auto lh::renderer::physical_extension_module::assert_required_extensions() -> bool
{
    const auto supported_extensions = physical_extension_module::supported_extensions();
    const auto required_extensions = physical_extension_module::required_extensions();

    for (const auto& required : required_extensions)
    {
        if (std::find_if(supported_extensions.begin(), supported_extensions.end(),
                         [&required](const auto& supported)
                         { return strcmp(required, supported.extensionName) == 0; }) == supported_extensions.end())
        {
            output::error() << "this system does not support the required vulkan physical extension: " +
                                   std::string {required};
            return false;
        }
    }

    return true;
}
