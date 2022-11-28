#include "renderer.hpp"

auto lh::renderer::initialize(window& window, const vulkan_version& version) -> void
{
    auto app_info = vk::ApplicationInfo {};
    auto instance_info = vk::InstanceCreateInfo {};

    // configure application info
    app_info.pApplicationName = window.get_title().data();
    app_info.applicationVersion = engine::get_version();
    app_info.pEngineName = app_info.pApplicationName;
    app_info.engineVersion = app_info.applicationVersion;
    app_info.apiVersion = VK_MAKE_API_VERSION(0, version.m_major, version.m_minor, version.m_patch);

    // make sure both extension and validation layers checks passed
    if (!(check_required_extensions() && check_required_validation_layers()))
    {
        output::fatal() << "this system does not support the required vulkan components";
    }

    auto required_extensions = renderer::required_extensions();
    auto required_validation_layers = renderer::required_validation_layers();

    // configure instance info
    instance_info.pApplicationInfo = &app_info;

    instance_info.enabledExtensionCount = required_extensions.size();
    instance_info.ppEnabledExtensionNames = required_extensions.data();

    instance_info.enabledLayerCount = m_using_validation_layers ? required_validation_layers.size() : 0;
    instance_info.ppEnabledLayerNames = m_using_validation_layers ? required_validation_layers.data() : nullptr;

    m_instance = vk::createInstance(instance_info);
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

auto lh::renderer::check_required_validation_layers() -> bool
{
    // if the rendere isn't using validation layers, check always passes
    if (!m_using_validation_layers)
        return true;

    // make sure the implementation supports all required validation layers
    auto required_layers = renderer::required_extensions();
    auto supported_layers = renderer::supported_extensions();
    auto layers_found = uint32_t {0};

    // cross check required and supported extensions
    for (auto& required : required_layers)
    {
        auto check = layers_found;

        for (const auto& supported : supported_layers)
            if (!strcmp(supported.extensionName, required))
            {
                layers_found += 1;
                break;
            }

        if (check == layers_found)
            output::error() << "this system does not support the required vulkan extension: " + std::string {required};
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

auto lh::renderer::supported_validation_layers() -> std::vector<vk::LayerProperties>
{
    // find the number of supported layers first
    auto num_layers = uint32_t {0};
    vk::enumerateInstanceLayerProperties(&num_layers, nullptr);

    auto layers = std::vector<vk::LayerProperties>(num_layers);
    vk::enumerateInstanceLayerProperties(&num_layers, layers.data());

    return layers;
}

auto lh::renderer::required_extensions() -> std::vector<const char*>
{
    // combine the extensions required by glfw with those specified in m_required_extensions
    auto num_extensions = uint32_t {0};
    auto glfw_extensions = vkfw::getRequiredInstanceExtensions(&num_extensions);

    auto combined_extensions = std::vector<const char*> {glfw_extensions, glfw_extensions + num_extensions};
    combined_extensions.insert(combined_extensions.end(), m_required_extensions.begin(), m_required_extensions.end());

    return combined_extensions;
}

auto lh::renderer::required_validation_layers() -> std::vector<const char*>
{
    return m_required_validation_layers;
}
