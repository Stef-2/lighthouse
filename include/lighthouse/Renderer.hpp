#pragma once

#include "SPIRV/GlslangToSpv.h"
#include "vkfw.hpp"
#include "vulkan.hpp"
#include "vulkan/utils/geometries.hpp"
#include "vulkan/utils/math.hpp"
#include "vulkan/utils/raii/raii_shaders.hpp"
#include "vulkan/utils/raii/raii_utils.hpp"
#include "vulkan/vulkan_to_string.hpp"
#include "vulkan_raii.hpp"

#include "datatype.hpp"
#include "output.hpp"
#include "version.hpp"
#include "window.hpp"
#include "memory.hpp"

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

        auto render() -> void;

    private:
        // optional module used for vulkan debugging
        struct validation_module
        {
            validation_module(vk::raii::Instance&);

            auto required_validation_layers() -> vk_string;
            auto supported_validation_layers() -> std::vector<vk::LayerProperties>;
            auto assert_required_validation_layers() -> bool;

            static VKAPI_ATTR auto VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                             void* pUserData) -> VkBool32;

            vk::raii::DebugUtilsMessengerEXT m_debug_messenger;

            static inline vk::DebugUtilsMessengerCreateInfoEXT m_debug_info {
                {},
                {vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError},
                {vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation},
                &debug_callback};

            static inline vk_string m_required_validation_layers {
                "VK_LAYER_KHRONOS_validation", "VK_LAYER_NV_optimus",
                //"VK_LAYER_NV_GPU_Trace_release_public_2021_4_0",
                //"VK_LAYER_VALVE_steam_fossilize",
                //"VK_LAYER_LUNARG_api_dump",
                //"VK_LAYER_LUNARG_gfxreconstruct",
                "VK_LAYER_KHRONOS_synchronization2", "VK_LAYER_LUNARG_monitor", "VK_LAYER_LUNARG_screenshot",
                "VK_LAYER_KHRONOS_profiles"};
        };

        // instance level vulkan extensions
        struct logical_extension_module
        {
            auto required_extensions() -> vk_string;
            auto supported_extensions() -> std::vector<vk::ExtensionProperties>;
            auto assert_required_extensions() -> bool;

            static inline vk_string m_required_extensions {"VK_EXT_debug_utils", "VK_EXT_debug_report"};
        };

        // physical device level vulkan extensions
        struct physical_extension_module
        {
            auto required_extensions() -> vk_string;
            auto supported_extensions() -> std::vector<vk::ExtensionProperties>;
            auto assert_required_extensions() -> bool;

            vk::raii::PhysicalDevice& m_device;
            static inline vk_string m_required_extensions {"VK_KHR_swapchain"};
        };

        auto create_context() -> vk::raii::Context;
        auto create_instance(const window&, const engine_version& = engine_version::m_default,
                             const vulkan_version& = vulkan_version::m_default, bool use_validaiton_module = true)
            -> vk::raii::Instance;

        auto create_physical_device() -> vk::raii::PhysicalDevice;
        auto create_surface(const window&) -> vk::raii::SurfaceKHR;
        auto create_extent(const window&) -> vk::Extent2D;
        auto create_surface_data(const window&) -> vk::raii::su::SurfaceData;
        auto create_graphics_family_queue_indices() -> std::pair<uint32_t, uint32_t>;
        auto create_device() -> vk::raii::Device;
        auto create_command_pool() -> vk::raii::CommandPool;
        auto create_command_buffer() -> vk::raii::CommandBuffer;
        auto create_graphics_queue() -> vk::raii::Queue;
        auto create_present_queue() -> vk::raii::Queue;
        auto create_swapchain(const window&) -> vk::raii::SwapchainKHR;
        auto create_swapchain_data(const window&) -> vk::raii::su::SwapChainData;
        auto create_depth_buffer(const window&) -> vk::raii::ImageView;
        auto create_depth_buffer_data(const window&) -> vk::raii::su::DepthBufferData;
        auto create_uniform_buffer() -> vk::raii::su::BufferData;
        auto create_descriptor_set_layout() -> vk::raii::DescriptorSetLayout;
        auto create_pipeline_layout() -> vk::raii::PipelineLayout;
        auto create_format() -> vk::Format;
        auto create_render_pass(const window&) -> vk::raii::RenderPass;
        auto create_shader_module(const vk::ShaderStageFlagBits&) -> vk::raii::ShaderModule;
        auto create_framebuffers(const window&) -> std::vector<vk::raii::Framebuffer>;
        auto create_vertex_buffer() -> vk::raii::su::BufferData;
        auto create_descriptor_pool() -> vk::raii::DescriptorPool;
        auto create_descriptor_set() -> vk::raii::DescriptorSet;
        auto create_pipeline_cache() -> vk::raii::PipelineCache;
        auto create_pipeline() -> vk::raii::Pipeline;
        auto create_image_views() -> std::vector<vk::raii::ImageView>;

        auto create_buffer(const data_t&, const vk::BufferUsageFlagBits&) -> vk::raii::Buffer;

        vulkan_version m_version;

        logical_extension_module m_logical_extensions;
        physical_extension_module m_physical_extensions;

        vk::raii::Context m_context;
        vk::raii::Instance m_instance;
        std::optional<validation_module> m_validation_module = {std::nullopt};
        vk::raii::PhysicalDevice m_physical_device;
        // vk::raii::su::SurfaceData m_surface_data;
        vk::raii::SurfaceKHR m_surface;
        vk::Extent2D m_extent;
        std::pair<uint32_t, uint32_t> m_graphics_and_present_queue_indices;
        vk::raii::Device m_device;
        vk::raii::CommandPool m_command_pool;
        vk::raii::CommandBuffer m_command_buffer;
        vk::raii::Queue m_graphics_queue;
        vk::raii::Queue m_present_queue;
        // vk::raii::SwapchainKHR m_swapchain;
        vk::raii::su::SwapChainData m_swapchain_data;
        // vk::raii::ImageView m_depth_buffer;
        vk::raii::su::DepthBufferData m_depth_buffer_data;
        vk::raii::su::BufferData m_uniform_buffer;
        vk::raii::DescriptorSetLayout m_descriptor_set_layout;
        vk::raii::PipelineLayout m_pipeline_layout;
        vk::Format m_format;
        vk::raii::RenderPass m_render_pass;
        vk::raii::ShaderModule m_shader_modules[2];
        std::vector<vk::raii::Framebuffer> m_framebuffers;
        vk::raii::su::BufferData m_vertex_buffer;
        vk::raii::DescriptorPool m_descriptor_pool;
        vk::raii::DescriptorSet m_descriptor_set;
        vk::raii::PipelineCache m_pipeline_cache;
        vk::raii::Pipeline m_pipeline;
        // std::vector<vk::raii::ImageView> m_image_views;
    };
}
