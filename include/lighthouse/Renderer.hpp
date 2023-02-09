#pragma once

#include "SPIRV/GlslangToSpv.h"
#include "vkfw.hpp"
#include "vma/vk_mem_alloc.hpp"
#include "vulkan.hpp"
#include "vulkan/utils/geometries.hpp"
#include "vulkan/utils/math.hpp"
#include "vulkan/utils/raii/raii_shaders.hpp"
#include "vulkan/utils/raii/raii_utils.hpp"
#include "vulkan/vulkan_to_string.hpp"
#include "vulkan_raii.hpp"

#include "datatype.hpp"
#include "memory.hpp"
#include "output.hpp"
#include "version.hpp"
#include "window.hpp"

#include <iterator>
#include <ranges>
#include <vector>

namespace lh
{
  class renderer
  {
  public:
	using vk_string_t = const std::vector<const char*>;
	using vk_layers_t = const std::vector<vk::LayerProperties>;
	using vk_extensions_t = const std::vector<vk::ExtensionProperties>;

	renderer(const window&,
			 const engine_version& = engine_version::m_default,
			 const vulkan_version& = vulkan_version::m_default,
			 bool use_validaiton_module = true);

	auto render() -> void;

	// ===========================================================================

  private:
	// vulkan object wrapper
	template <typename T> class vk_wrapper
	{
	public:
	  auto operator*() -> T& { return m_object; }
	  auto operator*() const -> T& { return m_object; }

	  auto operator->() -> T* { return &m_object; }
	  auto operator->() const -> T* { return &m_object; }

	  operator T&() { return m_object; }

	protected:
	  mutable T m_object {nullptr};
	};

	// optional module used for vulkan debugging
	struct validation_module
	{
	  validation_module(vk::raii::Instance&);

	  auto required_validation_layers() -> vk_string_t;
	  auto supported_validation_layers() -> std::vector<vk::LayerProperties>;
	  auto assert_required_validation_layers() -> bool;

	  static VKAPI_ATTR auto VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													   VkDebugUtilsMessageTypeFlagsEXT messageType,
													   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
													   void* pUserData) -> VkBool32;

	  vk::raii::DebugUtilsMessengerEXT m_debug_messenger;

	  struct defaults
	  {
		static inline vk::DebugUtilsMessengerCreateInfoEXT m_debug_info {
		  {},
		  {vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError},
		  {vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation},
		  &debug_callback};

		static inline const vk_string_t m_required_validation_layers {"VK_LAYER_KHRONOS_validation",
																	  "VK_LAYER_NV_optimus",
																	  //"VK_LAYER_NV_GPU_Trace_release_public_2021_4_0",
																	  //"VK_LAYER_VALVE_steam_fossilize",
																	  //"VK_LAYER_LUNARG_api_dump",
																	  //"VK_LAYER_LUNARG_gfxreconstruct",
																	  "VK_LAYER_KHRONOS_synchronization2",
																	  "VK_LAYER_LUNARG_monitor",
																	  "VK_LAYER_LUNARG_screenshot",
																	  "VK_LAYER_KHRONOS_profiles"};
	  };
	};

	// instance level vulkan extensions
	struct logical_extension_module
	{
	  auto required_extensions() -> vk_string_t;
	  auto supported_extensions() -> vk_extensions_t;
	  auto assert_required_extensions() -> bool;

	  struct defaults
	  {
		static inline const vk_string_t m_required_extensions {"VK_EXT_debug_utils",
															   "VK_KHR_get_physical_device_properties2",
															   "VK_KHR_get_surface_capabilities2"};
	  };
	};

	struct physical_device : public vk_wrapper<vk::raii::PhysicalDevice>
	{
	  using performance_score_t = uint64_t;

	  physical_device(const vk::raii::Instance&);

	  static auto get_performance_score(const vk::raii::PhysicalDevice&) -> performance_score_t;

	  auto required_extensions() const -> vk_string_t;
	  auto supported_extensions() const -> vk_extensions_t;
	  auto assert_required_extensions() -> bool;
	  auto get_basic_info() -> std::string;

	  struct defaults
	  {
		static inline const vk_string_t m_required_extensions {"VK_KHR_swapchain",
															   "VK_EXT_memory_budget",
															   "VK_KHR_portability_subset"};
		static inline constexpr performance_score_t m_minimum_accepted_score {0xFFFFFFFF};
	  };
	};

	struct logical_device : public vk_wrapper<vk::raii::Device>
	{

	  logical_device(const physical_device&,
					 const std::vector<vk::DeviceQueueCreateInfo>&,
					 const vk_string_t&,
					 const vk::PhysicalDeviceFeatures2& = {});

	  struct defaults
	  {
		static inline constexpr auto m_queue_priority = 1.0f;
		static inline constexpr auto m_properties = vk::PhysicalDeviceFeatures2 {};
	  };
	};

	// vulkan queue family indices
	struct queue_families
	{
	  using index_t = uint32_t;

	  index_t m_graphics;
	  index_t m_present;
	  index_t m_compute;
	  index_t m_transfer;
	};

	struct swapchain : public vk_wrapper<vk::raii::SwapchainKHR>
	{
	  swapchain(const physical_device&,
				const vk::raii::Device&,
				const vk::Extent2D&,
				const vk::raii::SurfaceKHR&,
				const queue_families&);

	  vk::SurfaceCapabilities2KHR m_surface_capabilities;
	  vk::SurfaceFormat2KHR m_surface_format;
	  vk::PresentModeKHR m_present_mode;

	  std::vector<vk::raii::ImageView> m_image_views;

	  struct defaults
	  {
		static inline const auto m_format = vk::SurfaceFormat2KHR {
		  {vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}};
		static inline constexpr auto m_present_mode = vk::PresentModeKHR::eImmediate;
		static inline constexpr auto m_image_count = uint32_t {2};
		static inline constexpr auto m_image_usage = vk::ImageUsageFlagBits::eColorAttachment;
		static inline constexpr auto m_sharing_mode = vk::SharingMode::eExclusive;
		static inline constexpr auto m_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
		static inline constexpr auto m_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		static inline constexpr auto m_image_view_type = vk::ImageViewType::e2D;
		static inline constexpr auto m_image_aspect = vk::ImageAspectFlagBits::eColor;
	  };
	};

	struct image
	{
	  struct defaults
	  {
		static inline constexpr auto m_format = vk::Format::eR8G8B8A8Srgb;

		static inline constexpr auto m_image_type = vk::ImageType::e2D;
		static inline constexpr auto m_image_create_flags = vk::ImageCreateFlags {};
		static inline constexpr auto m_image_sample_count = vk::SampleCountFlagBits::e1;
		static inline constexpr auto m_image_usage = vk::ImageUsageFlagBits::eSampled;
		static inline constexpr auto m_image_sharing_mode = vk::SharingMode::eExclusive;
		static inline constexpr auto m_image_layout = vk::ImageLayout::eAttachmentOptimal;
		static inline constexpr auto m_image_tiling = vk::ImageTiling::eOptimal;
		static inline constexpr auto m_image_aspect = vk::ImageAspectFlagBits::eColor;

		static inline constexpr auto m_view_type = vk::ImageViewType::e2D;

		static inline constexpr auto m_memory_type = vk::MemoryPropertyFlagBits::eDeviceLocal;
	  };

	  image(const physical_device&,
			const logical_device&,
			const VmaAllocator&,
			const vk::Extent2D&,
			const vk::Format& = defaults::m_format);

	  vk::Format m_format;
	  vk::raii::Image m_image;
	  vk::raii::ImageView m_view;
	  vk::raii::DeviceMemory m_memory;
	};

	struct depth_buffer : public vk_wrapper<vk::raii::ImageView>
	{
	  depth_buffer(const physical_device&, const logical_device&, const vk::Extent2D&);

	  struct defaults
	  {
		static inline constexpr auto m_tiling = vk::ImageTiling::eOptimal;
		static inline constexpr auto m_usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		static inline constexpr auto m_layout = vk::ImageLayout::eUndefined;
		static inline constexpr auto m_memory_property = vk::MemoryPropertyFlagBits::eDeviceLocal;
		static inline constexpr auto m_aspect = vk::ImageAspectFlagBits::eDepth;
		static inline constexpr auto m_format = vk::Format::eD16Unorm;
	  };
	};

	// vulkan memory allocator module
	struct memory_allocator_module
	{
	  memory_allocator_module(const vk::PhysicalDevice&,
							  const vk::Device&,
							  const vk::Instance&,
							  const engine_version& = engine_version::m_default);
	  ~memory_allocator_module();

	  operator VmaAllocator&();

	  VmaAllocator m_allocator;
	};

	auto create_context() -> vk::raii::Context;
	auto create_instance(const window&,
						 const engine_version& = engine_version::m_default,
						 const vulkan_version& = vulkan_version::m_default,
						 bool use_validaiton_module = true) -> vk::raii::Instance;

	auto create_physical_device() -> physical_device;
	auto create_surface(const window&) -> vk::raii::SurfaceKHR;
	auto create_extent(const window&) -> vk::Extent2D;
	auto create_queue_families() -> queue_families;
	auto create_device(const vk::PhysicalDeviceFeatures2& = {}) -> vk::raii::Device;
	auto create_command_pool() -> vk::raii::CommandPool;
	auto create_command_buffer() -> vk::raii::CommandBuffer;
	auto create_graphics_queue() -> vk::raii::Queue;
	auto create_present_queue() -> vk::raii::Queue;
	// auto create_swapchain(const window&) -> vk::raii::SwapchainKHR;
	// auto create_swapchain(const window&) -> swapchain;
	// auto create_swapchain_data(const window&) -> vk::raii::su::SwapChainData;
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
	// auto create_image_views() -> std::vector<vk::raii::ImageView>;

	auto create_buffer(const data_t&, const vk::BufferUsageFlagBits&) -> vk::raii::Buffer;

	template <typename T>
	  requires std::is_same_v<T, vk_layers_t> ||
			   std::is_same_v<T, vk_extensions_t>
			   static auto assert_required_components(T& supported, const vk_string_t& required_components) -> bool
	{

	  for (const auto& required : required_components)
	  {
		if (std::find_if(supported.begin(),
						 supported.end(),
						 [&required](const auto& supported)
						 {
						   auto name = std::string {};
						   if constexpr (std::is_same_v<T, vk_layers_t>)
							 name = supported.layerName.data();
						   else
							 name = supported.extensionName.data();

						   return strcmp(required, name.c_str()) == 0;
						 }) == supported.end())
		{
		  output::error() << "this system does not support the required vulkan component: " + std::string {required};
		  return false;
		}
	  }

	  return true;
	};

	vulkan_version m_version;

	logical_extension_module m_logical_extensions;

	vk::raii::Context m_context;
	vk::raii::Instance m_instance;
	std::optional<validation_module> m_validation_module = {std::nullopt};
	physical_device m_physical_device;
	// vk::raii::su::SurfaceData m_surface_data;
	vk::raii::SurfaceKHR m_surface;
	vk::Extent2D m_extent;
	queue_families m_queue_families;
	// vk::raii::Device m_device;
	logical_device m_device;
	vk::raii::CommandPool m_command_pool;
	vk::raii::CommandBuffer m_command_buffer;
	vk::raii::Queue m_graphics_queue;
	vk::raii::Queue m_present_queue;
	// vk::raii::SwapchainKHR m_swapchain;
	swapchain m_swapchain;
	// vk::raii::su::SwapChainData m_swapchain_data;
	//  vk::raii::ImageView m_depth_buffer;
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
	memory_allocator_module m_memory_allocator;
  };
}
