#include "renderer.hpp"

lh::renderer::renderer(const window& window, const engine_version& engine_version, const vulkan_version& vulkan_version,
                       bool use_validation_module)
    : m_version(vulkan_version),
      m_validation_module(use_validation_module ? validation_module {} : decltype(m_validation_module) {std::nullopt}),
      m_instance(create_instance(window, engine_version, vulkan_version, use_validation_module)),
      m_device {create_device()},
      m_command_buffer {create_command_buffer()}
{
    if (use_validation_module)
    {
        m_validation_module->m_debug_messenger = {m_instance, m_validation_module->m_debug_info};
    }
}

auto lh::renderer::extension_module::assert_required_extensions() -> bool
{
    // make sure the implementation supports all required extensions
    auto required_extensions = extension_module::required_extensions();
    auto supported_extensions = extension_module::supported_extensions();
    auto extensions_found = uint32_t {0};

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

auto lh::renderer::extension_module::supported_extensions() -> std::vector<vk::ExtensionProperties>
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

auto lh::renderer::extension_module::required_extensions() -> vk_string
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
    if (!(m_extension_module.assert_required_extensions() && validation_module))
    {
        output::fatal() << "this system does not support the required vulkan components";
    }

    auto required_extensions = m_extension_module.required_extensions();
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

auto lh::renderer::get_physical_device_extensions() -> vk_string
{
    const auto physical_extensions = vk::raii::PhysicalDevices(m_instance).front().enumerateDeviceExtensionProperties();
    auto physical_extension_names = vk_string {};
    
    std::ranges::for_each(physical_extensions.begin(), physical_extensions.end(),
                          [&physical_extension_names](auto& ext)
                          {
                              physical_extension_names.push_back(ext.extensionName);
                          });


    return physical_extension_names;
}

auto lh::renderer::create_device() -> vk::raii::Device
{
    const auto physical_device = vk::raii::PhysicalDevices(m_instance).front();

    const auto features_1 = physical_device.getFeatures();
    const auto features_2 = physical_device.getFeatures2();

    const auto physical_extensions = get_physical_device_extensions();/*
    for (const auto& e : physical_extensions)
    {
        std::cout << e << '\n';
    }*/
    //std::cout << physical_extensions.data();
    const auto queue_priority = 0.0f;
    auto device_queue_info = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = get_queue_family_index(),
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };
    std::cout << output::fatal();
    auto device_info = vk::DeviceCreateInfo {};
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &device_queue_info;
    device_info.pEnabledFeatures = &features_1;

    return {physical_device, device_info};
}

auto lh::renderer::create_command_buffer() -> vk::raii::CommandBuffer
{
    // create a command pool to allocate a command buffer from
    auto command_pool_info = vk::CommandPoolCreateInfo {.queueFamilyIndex = get_queue_family_index()};
    auto command_pool = vk::raii::CommandPool {m_device, command_pool_info};

    // allocate a CommandBuffer from the CommandPool
    auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo {
        .commandPool = *command_pool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1};

    vk::raii::CommandBuffer command_buffer =
        std::move(vk::raii::CommandBuffers(m_device, command_buffer_allocate_info).front());

    return command_buffer;
}

VKAPI_ATTR auto VKAPI_CALL lh::renderer::validation_module::debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32
{
    std::cout << "FUCK YOUR RETARDED MOTHER" << *pCallbackData->pMessage;
    return true;
}
