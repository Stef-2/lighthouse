#pragma once

#include "vkfw.hpp"
#include "vulkan.hpp"
#include "vulkan_raii.hpp"


#include "version.hpp"
#include "output.hpp"
#include "window.hpp"

#include <vector>

namespace lh
{
    class renderer
    {
      public:
        using vk_string = std::vector<const char*>;

        renderer(const window&, const engine_version& = engine_version::m_default,
                 const vulkan_version& = vulkan_version::m_default,
                 bool use_validaiton_module = true);

        // enumerate implementation supported extensions
        auto supported_extensions() -> std::vector<vk::ExtensionProperties>;

      private:
        // optional module used for vulkan debugging
        struct validation_module
        {
            auto required_validation_layers() -> vk_string;
            auto supported_validation_layers() -> std::vector<vk::LayerProperties>;
            auto check_required_validation_layers() -> bool;

            vk::raii::DebugUtilsMessengerEXT m_debug_messenger {nullptr};
            vk_string m_required_validation_layers = {"VK_LAYER_KHRONOS_validation"};
        };

        // enumerate required extensions layers
        auto required_extensions() -> vk_string;
        // check if the system supports required extensions
        auto check_required_extensions() -> bool;

        vk::raii::Instance m_instance {nullptr};
        vk::raii::SurfaceKHR m_surface {nullptr};

        vulkan_version m_version;
        std::optional<validation_module> m_validation_module = {std::nullopt};

        vk_string m_required_extensions = {"VK_EXT_debug_utils"};
        
        static VKAPI_ATTR auto VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                         VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                         const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                         void* pUserData) ->VkBool32;


    };
}
