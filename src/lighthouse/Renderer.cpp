#include "renderer.hpp"

lh::renderer::renderer(const window& window, const engine_version& engine_version, const vulkan_version& vulkan_version,
                       bool use_validation_module)
    : m_version {vulkan_version},
      m_validation_module {use_validation_module ? validation_module {} : decltype(m_validation_module) {std::nullopt}},
      m_instance {create_instance(window, engine_version, vulkan_version, use_validation_module)},
      m_physical_device {create_physical_device()},
      m_physical_extensions {m_physical_device},
      m_device {create_device()},
      m_command_buffer {create_command_buffer()},
      m_surface {create_surface(window)},
      m_swapchain {create_swapchain(window)},
      m_image_views {create_image_views()},
      m_depth_buffer {create_depth_buffer(window)}
{
    if (use_validation_module)
    {
        m_validation_module->m_debug_messenger = {m_instance, m_validation_module->m_debug_info};
    }
}

auto lh::renderer::logical_extension_module::assert_required_extensions() -> bool
{
    // make sure the implementation supports all required extensions
    auto required_extensions = logical_extension_module::required_extensions();
    auto supported_extensions = logical_extension_module::supported_extensions();
    auto extensions_found = uint32_t {0};

    for (const auto& ext : supported_extensions)
        std::cout << ext.extensionName << '\n';

    // cross check required and supported extensions
    for (auto& required : required_extensions)
    {
        auto check = extensions_found;

        for (const auto& supported : supported_extensions)
            if (!strcmp(supported.extensionName, required))
            {
                extensions_found += 1;
                break;
            }

        if (check == extensions_found)
            output::error() << "this system does not support the required vulkan extension: " + std::string {required};
    }

    return extensions_found == required_extensions.size();
}

auto lh::renderer::validation_module::assert_required_validation_layers() -> bool
{
    // make sure the implementation supports all required validation layers
    auto required_layers = validation_module::required_validation_layers();
    auto supported_layers = validation_module::supported_validation_layers();
    auto layers_found = uint32_t {0};

    // cross check required and supported extensions
    for (auto& required : required_layers)
    {
        auto check = layers_found;

        for (const auto& supported : supported_layers)
            if (!strcmp(supported.layerName, required))
            {
                layers_found += 1;
                break;
            }

        if (check == layers_found)
            output::error() << "this system does not support the required vulkan validation layer: " +
                                   std::string {required};
    }

    return layers_found == required_layers.size();
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

auto lh::renderer::validation_module::required_validation_layers() -> vk_string
{
    return m_required_validation_layers;
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

    if (m_validation_module)
        validation_module = m_validation_module->assert_required_validation_layers();

    // make sure both extension and validation layers checks passed
    if (!(m_logical_extensions.assert_required_extensions() && validation_module))
    {
        output::fatal() << "this system does not support the required vulkan components";
    }

    auto required_extensions = m_logical_extensions.required_extensions();
    auto required_validation_layers =
        m_validation_module ? m_validation_module->required_validation_layers() : vk_string {nullptr};

    // configure instance info
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount = required_extensions.size();
    instance_info.ppEnabledExtensionNames = required_extensions.data();
    instance_info.enabledLayerCount = m_validation_module ? required_validation_layers.size() : 0;
    instance_info.ppEnabledLayerNames = m_validation_module ? required_validation_layers.data() : nullptr;

    return {{}, instance_info};
}

auto lh::renderer::get_queue_family_index() -> uint32_t
{
    const auto physical_device = vk::raii::PhysicalDevices(m_instance).front();
    const auto queue_family_properties = physical_device.getQueueFamilyProperties();

    // get the first index into queue_family_properties which supports graphics
    std::vector<vk::QueueFamilyProperties>::const_iterator queue_family_property = std::find_if(
        queue_family_properties.begin(), queue_family_properties.end(),
        [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; });

    // if none of the available devices support the queue family, terminate
    if (queue_family_property == queue_family_properties.end())
        output::fatal() << "this system does not support the required vulkan components";

    return static_cast<uint32_t>(std::distance(queue_family_properties.begin(), queue_family_property));
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
    device_queue_info.queueFamilyIndex = get_queue_family_index();
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

auto lh::renderer::create_physical_device() -> vk::raii::PhysicalDevice
{
    return vk::raii::PhysicalDevices(m_instance).front();
}

auto lh::renderer::create_command_buffer() -> vk::raii::CommandBuffer
{
    // create a command pool to allocate a command buffer from
    auto command_pool_info = vk::CommandPoolCreateInfo {};
    command_pool_info.queueFamilyIndex = get_queue_family_index();

    auto command_pool = vk::raii::CommandPool {m_device, command_pool_info};

    // allocate a command_buffer from the command_pool
    auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo {};
    command_buffer_allocate_info.commandPool = *command_pool;
    command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
    command_buffer_allocate_info.commandBufferCount = 1;

    return std::move(vk::raii::CommandBuffers(m_device, command_buffer_allocate_info).front());
}

auto lh::renderer::create_surface(const window& window) -> vk::raii::SurfaceKHR
{
    auto surface = VkSurfaceKHR {};
    glfwCreateWindowSurface(static_cast<VkInstance>(*m_instance), window.vkfw_window(), nullptr, &surface);

    return {m_instance, surface};
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
        swapchain_extent.width = glm::clamp(window.get_resolution().first, surface_capabilities.minImageExtent.width,
                                            surface_capabilities.maxImageExtent.width);
        swapchain_extent.height = glm::clamp(window.get_resolution().second, surface_capabilities.minImageExtent.height,
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

    return swapchain; //{m_device, swapchain_info};
}

auto lh::renderer::create_image_views() -> std::vector<vk::raii::ImageView>
{
    auto swapchain_images = m_swapchain.getImages();

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

    auto surface_data =
        vk::su::SurfaceData(*m_instance, window.get_title().data(),
                            vk::Extent2D {window.get_resolution().first, window.get_resolution().second});

    auto image_info = vk::ImageCreateInfo {};
    image_info.imageType = vk::ImageType::e2D;
    image_info.format = depth_format;
    image_info.extent = vk::Extent3D {surface_data.extent.width, surface_data.extent.height, 1};

    auto image = vk::raii::Image{m_device, image_info};

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

VKAPI_ATTR auto VKAPI_CALL lh::renderer::validation_module::debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32
{
    std::cout << "FUCK YOUR RETARDED MOTHER" << *pCallbackData->pMessage;
    return true;
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

    auto physical_extension_names = vk_string {};

    std::ranges::for_each(supported_extensions.begin(), supported_extensions.end(),
                          [&physical_extension_names](auto& ext)
                          {
                              physical_extension_names.push_back(ext.extensionName);
                          });

    auto extensions_found = uint32_t {0};

    for (const auto& ext : supported_extensions)
        std::cout << ext.extensionName << '\n';

    // cross check required and supported extensions
    for (auto& required : required_extensions)
    {
        auto check = extensions_found;

        for (const auto& supported : supported_extensions)
            if (!strcmp(supported.extensionName, required))
            {
                extensions_found += 1;
                break;
            }

        if (check == extensions_found)
            output::error() << "this system does not support the required vulkan extension: " + std::string {required};
    }

    return extensions_found == required_extensions.size();
}
