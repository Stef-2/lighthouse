#pragma once

#include "vkfw.hpp"
#include "vulkan.hpp"
#include "vulkan_raii.hpp"
#include "vulkan/utils/utils.hpp"

#include "datatype.hpp"
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
        using vk_string = const std::vector<const char*>;

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
                vk::DebugUtilsMessengerCreateFlagsEXT {},
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
               debug_callback};

            vk::raii::DebugUtilsMessengerEXT m_debug_messenger {nullptr};
            vk_string m_required_validation_layers {"VK_LAYER_KHRONOS_validation"};
        };

        // instance level vulkan extensions
        struct logical_extension_module {
            auto required_extensions() -> vk_string;
            auto supported_extensions() -> std::vector<vk::ExtensionProperties>;
            auto assert_required_extensions() -> bool;

            vk_string m_required_extensions {"VK_EXT_debug_utils"};
        };

        // physical device level vulkan extensions
        struct physical_extension_module {

            auto required_extensions() -> vk_string;
            auto supported_extensions() -> std::vector<vk::ExtensionProperties>;
            auto assert_required_extensions() -> bool;

            vk::raii::PhysicalDevice& m_device;
            vk_string m_required_extensions {"VK_KHR_swapchain"};
        };

        auto get_queue_family_index() -> uint32_t;

        auto create_instance(const window&, const engine_version& = engine_version::m_default,
                             const vulkan_version& = vulkan_version::m_default, bool use_validaiton_module = true)
            -> vk::raii::Instance;

        auto create_device() -> vk::raii::Device;
        auto create_physical_device() -> vk::raii::PhysicalDevice;
        auto create_command_buffer() -> vk::raii::CommandBuffer;
        auto create_surface(const window&) ->vk::raii::SurfaceKHR;
        auto create_swapchain(const window&) -> vk::raii::SwapchainKHR;
        auto create_image_views() -> std::vector<vk::raii::ImageView>;
        auto create_depth_buffer(const window&) -> vk::raii::ImageView;

        auto create_buffer(const data_t data) -> vk::raii::Buffer;

        vulkan_version m_version;

        std::optional<validation_module> m_validation_module = {std::nullopt};
        logical_extension_module m_logical_extensions;
        physical_extension_module m_physical_extensions;

        vk::raii::Instance m_instance;
        vk::raii::PhysicalDevice m_physical_device;
        vk::raii::Device m_device;
        vk::raii::CommandBuffer m_command_buffer;
        vk::raii::SurfaceKHR m_surface;
        vk::raii::SwapchainKHR m_swapchain;
        std::vector<vk::raii::ImageView> m_image_views;
        vk::raii::ImageView m_depth_buffer;
    };
}
