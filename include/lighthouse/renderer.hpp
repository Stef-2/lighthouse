#pragma once

#include "vulkan.hpp"

#include <iterator>
#include <ranges>
#include <vector>

namespace lh
{
	class renderer
	{
	public:
		struct create_info
		{
			version m_engine_version = version::m_engine_version;
			version m_vulkan_version = version::m_vulkan_version;
			uint32_t m_frames_in_flight = 2;
			bool m_using_validation = true;
		};

		renderer(const window&, const create_info& = {});

		auto render() -> void;

		// ===========================================================================

	private:
		// optional module used for vulkan debugging
		struct validation_module : public vulkan::vk_wrapper<vk::raii::DebugUtilsMessengerEXT>
		{
			validation_module(vk::raii::Instance&);

			auto required_validation_layers() const -> vulkan::vk_string_t;
			auto supported_validation_layers() const -> std::vector<vk::LayerProperties>;
			auto assert_required_validation_layers() -> bool;
			auto info() const -> output::string_t;
			static VKAPI_ATTR auto VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
															 VkDebugUtilsMessageTypeFlagsEXT messageType,
															 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
															 void* pUserData) -> VkBool32;

			struct create_info
			{
				vk::DebugUtilsMessengerCreateInfoEXT m_debug_info {
					{},
					{vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
					 vk::DebugUtilsMessageSeverityFlagBitsEXT::eError},
					{vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
					 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
					 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
					 vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding},
					&debug_callback};

				vulkan::vk_string_t m_required_validation_layers {"VK_LAYER_KHRONOS_validation",
																  "VK_LAYER_NV_optimus",
																  //"VK_LAYER_NV_GPU_Trace_release_public_2021_4_0",
																  //"VK_LAYER_VALVE_steam_fossilize",
																  //"VK_LAYER_LUNARG_api_dump",
																  //"VK_LAYER_LUNARG_gfxreconstruct",
																  "VK_LAYER_KHRONOS_synchronization2",
																  "VK_LAYER_LUNARG_monitor",
																  "VK_LAYER_LUNARG_screenshot",
																  "VK_LAYER_KHRONOS_profiles"};
			} static inline const m_defaults;
		};

		// instance level vulkan extensions
		struct logical_extension_module
		{
			struct create_info
			{
				vulkan::vk_string_t m_required_extensions {"VK_EXT_debug_utils",
														   "VK_KHR_get_physical_device_properties2",
														   "VK_KHR_get_surface_capabilities2"};
			} static inline const m_defaults;

			auto required_extensions() const -> vulkan::vk_string_t;
			auto supported_extensions() const -> vulkan::vk_extensions_t;
			auto assert_required_extensions() const -> bool;
			auto info() const -> output::string_t;
		};

		struct physical_device : public vulkan::vk_wrapper<vk::raii::PhysicalDevice>
		{
			using performance_score_t = uint64_t;

			struct create_info
			{
				vulkan::vk_string_t m_required_extensions {"VK_KHR_swapchain",
														   "VK_EXT_memory_budget",
														   "VK_KHR_portability_subset"};
				performance_score_t m_minimum_accepted_score {0xFFFFFFFF};
			} static inline const m_defaults;

			physical_device(const vk::raii::Instance&, const create_info& = m_defaults);

			static auto get_performance_score(const vk::raii::PhysicalDevice&) -> performance_score_t;

			auto required_extensions() const -> vulkan::vk_string_t;
			auto supported_extensions() const -> vulkan::vk_extensions_t;
			auto assert_required_extensions() -> bool;
			auto basic_info() const -> output::string_t;
			auto advanced_info() const -> output::string_t;
		};

		struct logical_device : public vulkan::vk_wrapper<vk::raii::Device>
		{

			struct create_info
			{
				float m_queue_priority = 1.0f;
				vk::PhysicalDeviceFeatures2 m_features = vk::PhysicalDeviceFeatures2 {};
			} static inline const m_defaults;

			logical_device(const physical_device&,
						   const std::vector<vk::DeviceQueueCreateInfo>&,
						   const vulkan::vk_string_t&,
						   const create_info& = m_defaults);
		};

		// vulkan memory allocator module
		struct memory_allocator
		{
			memory_allocator(const vk::Instance&,
							 const physical_device&,
							 const logical_device&,
							 const version& = version::m_engine_version);
			~memory_allocator();

			operator vma::Allocator&();

			vma::Allocator m_allocator;
		};

		// vulkan queue family indices
		struct queue_families
		{
			using index_t = uint32_t;

			struct create_info
			{

			} static inline const m_defaults;

			queue_families(const physical_device&, const vk::raii::SurfaceKHR&, const create_info& = m_defaults);

			index_t m_graphics;
			index_t m_present;
			index_t m_compute;
			index_t m_transfer;
		};

		struct image : public vulkan::vk_wrapper<vk::raii::Image>
		{
			struct create_info
			{
				vk::Format m_format = vk::Format::eR8G8B8A8Srgb;

				vk::ImageType m_image_type = vk::ImageType::e2D;
				vk::ImageCreateFlags m_image_create_flags = vk::ImageCreateFlags {};
				vk::SampleCountFlagBits m_image_sample_count = vk::SampleCountFlagBits::e1;
				vk::ImageUsageFlagBits m_image_usage = vk::ImageUsageFlagBits::eSampled;
				vk::SharingMode m_image_sharing_mode = vk::SharingMode::eExclusive;
				vk::ImageLayout m_image_layout = vk::ImageLayout::eAttachmentOptimal;
				vk::ImageTiling m_image_tiling = vk::ImageTiling::eOptimal;
				vk::ImageAspectFlagBits m_image_aspect = vk::ImageAspectFlagBits::eColor;

				vk::ImageViewType m_view_type = vk::ImageViewType::e2D;

				vk::MemoryPropertyFlagBits m_memory_type = vk::MemoryPropertyFlagBits::eDeviceLocal;
			} static inline const m_defaults;

			image(const physical_device&,
				  const logical_device&,
				  const memory_allocator&,
				  const vk::Extent2D&,
				  const create_info& = m_defaults);

			vk::Format m_format;
			vk::raii::ImageView m_view;
			vk::raii::DeviceMemory m_memory;
		};

		struct depth_buffer : public image
		{
			using image::image;

			static inline const create_info m_defaults {
				.m_format = vk::Format::eD16Unorm,
				.m_image_usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
				.m_image_layout = vk::ImageLayout::eUndefined,
				.m_image_tiling = vk::ImageTiling::eOptimal,
				.m_image_aspect = vk::ImageAspectFlagBits::eDepth,
				.m_memory_type = vk::MemoryPropertyFlagBits::eDeviceLocal,
			};
		};

		struct renderpass : public vulkan::vk_wrapper<vk::raii::RenderPass>
		{
			struct create_info
			{
				vk::SampleCountFlagBits m_sample_count = vk::SampleCountFlagBits::e1;
				vk::AttachmentLoadOp m_load_operation = vk::AttachmentLoadOp::eClear;

				vk::AttachmentDescription m_color_attachment = {{},
																vk::Format::eB8G8R8A8Srgb,
																m_sample_count,
																m_load_operation,
																vk::AttachmentStoreOp::eStore,
																vk::AttachmentLoadOp::eDontCare,
																vk::AttachmentStoreOp::eDontCare,
																vk::ImageLayout::eUndefined,
																vk::ImageLayout::ePresentSrcKHR};

				vk::AttachmentDescription m_depth_attachment = {{},
																vk::Format::eD16Unorm,
																m_sample_count,
																m_load_operation,
																vk::AttachmentStoreOp::eDontCare,
																vk::AttachmentLoadOp::eDontCare,
																vk::AttachmentStoreOp::eDontCare,
																vk::ImageLayout::eUndefined,
																vk::ImageLayout::eDepthStencilAttachmentOptimal};

				vk::AttachmentReference m_color_attachment_reference = {0, vk::ImageLayout::eColorAttachmentOptimal};
				vk::AttachmentReference m_depth_attachment_reference = {
					1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

				vk::PipelineBindPoint m_bind_point = vk::PipelineBindPoint::eGraphics;

			} static inline const m_defaults;

			renderpass(const physical_device&,
					   const logical_device&,
					   const vk::raii::SurfaceKHR&,
					   const create_info& = m_defaults);
		};

		struct framebuffer : public vulkan::vk_wrapper<vk::raii::Framebuffer>
		{
			struct create_info
			{

			} static inline const m_defaults;

			framebuffer(const logical_device&,
						const renderpass&,
						const image&,
						const depth_buffer&,
						const vk::Extent2D,
						const create_info& = m_defaults);

			framebuffer(const logical_device&,
						const renderpass&,
						const vk::raii::ImageView&,
						const vk::raii::ImageView&,
						const vk::Extent2D,
						const create_info& = m_defaults);
		};

		struct swapchain : public vulkan::vk_wrapper<vk::raii::SwapchainKHR>
		{
			struct create_info
			{
				vk::SurfaceFormat2KHR m_format = {{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}};
				vk::PresentModeKHR m_present_mode = vk::PresentModeKHR::eImmediate;
				uint32_t m_image_count = {2};
				vk::ImageUsageFlagBits m_image_usage = vk::ImageUsageFlagBits::eColorAttachment;
				vk::SharingMode m_sharing_mode = vk::SharingMode::eExclusive;
				vk::SurfaceTransformFlagBitsKHR m_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
				vk::CompositeAlphaFlagBitsKHR m_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
				vk::ImageViewType m_image_view_type = vk::ImageViewType::e2D;
				vk::ImageAspectFlagBits m_image_aspect = vk::ImageAspectFlagBits::eColor;
			} static inline const m_defaults;

			swapchain(const physical_device&,
					  const logical_device&,
					  const vk::Extent2D&,
					  const vk::raii::SurfaceKHR&,
					  const queue_families&,
					  const memory_allocator&,
					  const renderpass&,
					  const create_info& = m_defaults);

			vk::SurfaceCapabilities2KHR m_surface_capabilities;
			vk::SurfaceFormat2KHR m_surface_format;
			vk::PresentModeKHR m_present_mode;

			depth_buffer m_depth_buffer;
			std::vector<vk::raii::ImageView> m_image_views;
			std::vector<framebuffer> m_framebuffers;
		};

		struct buffer : public vulkan::vk_wrapper<vk::raii::Buffer>
		{
			struct create_info
			{
				vk::BufferUsageFlags m_usage = {};
				vk::MemoryPropertyFlags m_properties = vk::MemoryPropertyFlagBits::eHostVisible |
													   vk::MemoryPropertyFlagBits::eHostCoherent;
			} static const inline m_defaults;

			buffer(const physical_device&,
				   const logical_device&,
				   const vma::Allocator&,
				   const vk::DeviceSize&,
				   const create_info& = m_defaults);

			vk::raii::DeviceMemory m_memory;
		};

		// e1m4
		struct command_control : public vulkan::vk_wrapper<vk::raii::CommandPool>
		{
			struct create_info
			{
				vk::CommandPoolCreateFlags m_pool_flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

				vk::CommandBufferLevel m_buffer_level = vk::CommandBufferLevel::ePrimary;
				uint32_t m_num_buffers = {1};

			} static inline const m_defaults;

			command_control(const logical_device&, const queue_families&, const create_info& = m_defaults);

			vk::raii::CommandBuffers m_buffers;
		};
		vk::PhysicalDeviceDescriptorBufferPropertiesEXT asd;
		struct descriptor_set_layout : public vulkan::vk_wrapper<vk::raii::DescriptorSetLayout>
		{
			using descriptor_count_t = uint32_t;
			using binding_location_t = uint32_t;

			struct binding
			{
				binding_location_t m_location {};
				vk::DescriptorType m_type {};
				descriptor_count_t m_count {1};
			};

			struct create_info
			{
				vk::DescriptorSetLayoutCreateFlags m_flags {};
				vk::ShaderStageFlagBits m_access = vk::ShaderStageFlagBits::eAll;

			} static inline const m_defaults;

			descriptor_set_layout(const logical_device&, const std::vector<binding>&, const create_info& = m_defaults);
		};

		struct descriptor_pool : public vulkan::vk_wrapper<vk::raii::DescriptorPool>
		{
			struct create_info
			{
				vk::DescriptorPoolCreateFlags m_flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
			} static inline const m_defaults;

			descriptor_pool();
		};

		// ===========================================================================

		auto create_context() -> vk::raii::Context;
		auto create_instance(const window&,
							 const version& = version::m_engine_version,
							 const version& = version::m_vulkan_version,
							 bool use_validaiton_module = true) -> vk::raii::Instance;

		auto create_physical_device() -> physical_device;
		auto create_surface(const window&) -> vk::raii::SurfaceKHR;
		auto create_extent(const window&) -> vk::Extent2D;

		auto create_command_pool() -> vk::raii::CommandPool;
		auto create_command_buffer() -> vk::raii::CommandBuffer;
		auto create_graphics_queue() -> vk::raii::Queue;
		auto create_present_queue() -> vk::raii::Queue;

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
		auto info(const create_info& = {}) -> output::string_t;

		template <typename T>
			requires std::is_same_v<T, vulkan::vk_layers_t> ||
					 std::is_same_v<T, vulkan::vk_extensions_t>
					 static auto assert_required_components(T& supported,
															const vulkan::vk_string_t& required_components) -> bool
		{

			for (const auto& required : required_components)
			{
				if (std::find_if(supported.begin(), supported.end(), [&required](const auto& supported) {
						auto name = std::string {};
						if constexpr (std::is_same_v<T, vulkan::vk_layers_t>)
							name = supported.layerName.data();
						else
							name = supported.extensionName.data();

						return strcmp(required, name.c_str()) == 0;
					}) == supported.end())
				{
					output::error() << "this system does not support the required vulkan component: " +
										   std::string {required};
					return false;
				}
			}

			return true;
		};

		version m_version;

		logical_extension_module m_logical_extensions;

		vk::raii::Context m_context;
		vk::raii::Instance m_instance;
		std::optional<validation_module> m_validation_module = {std::nullopt};
		physical_device m_physical_device;

		vk::raii::SurfaceKHR m_surface;
		vk::Extent2D m_extent;
		queue_families m_queue_families;

		logical_device m_device;
		memory_allocator m_memory_allocator;
		command_control m_command_control;

		vk::raii::Queue m_graphics_queue;
		vk::raii::Queue m_present_queue;

		renderpass m_renderpass;
		swapchain m_swapchain;

		buffer m_uniform_buffer;

		// vk::raii::DescriptorSetLayout m_descriptor_set_layout;
		descriptor_set_layout m_descriptor_set_layout;
		vk::raii::DescriptorPool m_descriptor_pool;
		vk::raii::DescriptorSet m_descriptor_set;
		static constexpr auto b = sizeof(VkDescriptorPool);
		vk::raii::ShaderModule m_shader_modules[2];
		vk::raii::PipelineLayout m_pipeline_layout;
		vk::raii::PipelineCache m_pipeline_cache;
		vk::raii::Pipeline m_pipeline;

		vk::raii::su::BufferData m_vertex_buffer;
		vulkan::instance inst;
	};
}
