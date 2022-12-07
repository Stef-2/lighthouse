#include "renderer.hpp"

lh::renderer::renderer(const window& window, const engine_version& engine_version, const vulkan_version& vulkan_version, bool use_validation_module)
    : m_version(vulkan_version), m_validation_module{}
{
    auto app_info = vk::ApplicationInfo {};
    auto instance_info = vk::InstanceCreateInfo {};
    auto debug_info = vk::DebugUtilsMessengerCreateInfoEXT {};
    auto validation_module = true;

    // configure application info
    app_info.pApplicationName = window.get_title().data();
    app_info.applicationVersion = engine_version;
    app_info.pEngineName = app_info.pApplicationName;
    app_info.engineVersion = app_info.applicationVersion;
    app_info.apiVersion =
        VK_MAKE_API_VERSION(0, vulkan_version.m_major, vulkan_version.m_minor, vulkan_version.m_patch);
    
    if (m_validation_module)
        validation_module = m_validation_module->check_required_validation_layers();

    // make sure both extension and validation layers checks passed
    if (!(check_required_extensions() && validation_module))
    {
        output::fatal() << "this system does not support the required vulkan components";
    }

    auto required_extensions = renderer::required_extensions();
    auto required_validation_layers = m_validation_module ? m_validation_module->required_validation_layers() : vk_string{nullptr};

    // configure instance info
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount = required_extensions.size();
    instance_info.ppEnabledExtensionNames = required_extensions.data();
    instance_info.enabledLayerCount = m_validation_module ? required_validation_layers.size() : 0;
    instance_info.ppEnabledLayerNames = m_validation_module ? required_validation_layers.data() : nullptr;

    m_instance = {{}, instance_info};
}

auto lh::renderer::check_required_extensions() -> bool
{
    // make sure the implementation supports all required extensions
    auto required_extensions = renderer::required_extensions();
    auto supported_extensions = renderer::supported_extensions();
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

auto lh::renderer::validation_module::check_required_validation_layers() -> bool
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
            output::error() << "this system does not support the required vulkan validation layer: " + std::string {required};
    }

    return layers_found == required_layers.size();
}

auto lh::renderer::supported_extensions() -> std::vector<vk::ExtensionProperties>
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

auto lh::renderer::required_extensions() -> vk_string
{
    // combine the extensions required by glfw with those specified in m_required_extensions
    auto num_extensions = uint32_t {0};
    auto glfw_extensions = vkfw::getRequiredInstanceExtensions(&num_extensions);

    auto combined_extensions = std::vector<const char*> {glfw_extensions, glfw_extensions + num_extensions};
    combined_extensions.insert(combined_extensions.end(), m_required_extensions.begin(), m_required_extensions.end());

    return combined_extensions;
}

auto lh::renderer::validation_module::required_validation_layers() -> vk_string
{
    return m_required_validation_layers;
}

VKAPI_ATTR auto VKAPI_CALL lh::renderer::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData) -> VkBool32
{
    return false;
}
