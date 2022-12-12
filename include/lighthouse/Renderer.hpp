#pragma once

#include "vkfw.hpp"
#include "vulkan.hpp"
#include "vulkan_raii.hpp"

#include "output.hpp"
#include "version.hpp"
#include "window.hpp"

#include <iterator>
#include <vector>

namespace lh
{
    class renderer
    {
    public:
        using vk_string = std::vector<const char*>;

        renderer(const window&, const engine_version& = engine_version::m_default,
                 const vulkan_version& = vulkan_version::m_default, bool use_validaiton_module = true);

    private:
        // optional module used for vulkan debugging
        struct validation_module {
            auto required_validation_layers() -> vk_string;
            auto supported_validation_layers() -> std::vector<vk::LayerProperties>;
            auto assert_required_validation_layers() -> bool;

            static VKAPI_ATTR auto VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                             void* pUserData) -> VkBool32;

            vk::DebugUtilsMessengerCreateInfoEXT m_debug_info {
                .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
                .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
                .pfnUserCallback = debug_callback};

            vk::raii::DebugUtilsMessengerEXT m_debug_messenger {nullptr};
            vk_string m_required_validation_layers {"VK_LAYER_KHRONOS_validation"};
        };

        struct extension_module {
            auto required_extensions() -> vk_string;
            auto supported_extensions() -> std::vector<vk::ExtensionProperties>;
            auto assert_required_extensions() -> bool;

            vk_string m_required_extensions {"VK_EXT_debug_utils"};
        };

        auto create_instance(const window&, const engine_version& = engine_version::m_default,
                             const vulkan_version& = vulkan_version::m_default, bool use_validaiton_module = true)
            -> vk::raii::Instance;

        auto get_queue_family_index() -> uint32_t;
        auto get_physical_device_extensions() -> vk_string;

        auto create_device() -> vk::raii::Device;
        auto create_command_buffer() -> vk::raii::CommandBuffer;

        vulkan_version m_version;

        std::optional<validation_module> m_validation_module = {std::nullopt};
        extension_module m_extension_module {};

        vk::raii::Instance m_instance {nullptr};
        vk::raii::Device m_device {nullptr};
        vk::raii::CommandBuffer m_command_buffer {nullptr};
    };
}
