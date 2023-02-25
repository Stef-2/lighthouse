#pragma once

#include "SPIRV/GlslangToSpv.h"
#include "vkfw/vkfw.hpp"
#include "vma/vk_mem_alloc.hpp"
#include "vulkan/utils/geometries.hpp"
#include "vulkan/utils/math.hpp"
#include "vulkan/utils/raii/raii_shaders.hpp"
#include "vulkan/utils/raii/raii_utils.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include "vulkan/vulkan_to_string.hpp"

#include "output.hpp"
#include "window.hpp"
#include "version.hpp"
#include "datatype.hpp"
#include "memory.hpp"

#include <ranges>

namespace lh
{
	namespace vulkan
	{
		using vk_string_t = const std::vector<const char*>;
		using vk_layers_t = const std::vector<vk::LayerProperties>;
		using vk_extensions_t = const std::vector<vk::ExtensionProperties>;

		static inline constexpr auto vk_whole_size = VK_WHOLE_SIZE;

		// ==========================================================================
		// base vulkan::raii object wrapper
		// ==========================================================================
		template <typename T> class vk_wrapper
		{
		public:
			auto operator*() -> T& { return m_object; }
			auto operator*() const -> T& { return m_object; }

			auto operator->() -> T* { return &m_object; }
			auto operator->() const -> T* { return &m_object; }

			operator T&() { return m_object; }

			virtual auto info() const -> output::string_t { return {}; }

		protected:
			vk_wrapper(const T&) = delete;
			vk_wrapper operator=(const T&) = delete;

			vk_wrapper(T&& object) noexcept : m_object(std::move(object)) {};
			vk_wrapper(nullptr_t object = nullptr) : m_object(object) {};

			mutable T m_object;
		};

		// ==========================================================================
		// abstract base for vulkan logical phyisical and validation extensions
		// ==========================================================================
		template <typename T>
			requires std::is_same_v<T, vk_layers_t> or std::is_same_v<T, vk_extensions_t>
		class vulkan_extension_module
		{
		public:
			struct create_info
			{
				vk_string_t m_required_extensions {};
			};

			vulkan_extension_module(const T& supported, const create_info& create_info)
				: m_supported_extensions(supported), m_required_extensions(create_info.m_required_extensions)
			{
				if (not assert_required_extensions())
					output::error() << "this system does not support the required " + extension_type() + "s";
			}

			auto required_extensions() const -> vk_string_t { return m_required_extensions; }
			auto supported_extensions() const -> T { return m_supported_extensions; }
			auto assert_required_extensions() const -> bool
			{
				const auto supported = supported_extensions();

				for (const auto& required : required_extensions())
				{
					if (std::find_if(supported.begin(), supported.end(), [&required, this](const auto& supported) {
							return std::strcmp(required, extension_name(supported).c_str()) == 0;
						}) == supported.end())
					{
						output::error() << "this system does not support the required " + extension_type() + ": " +
											   output::string_t {required};
						return false;
					}
				}

				return true;
			}
			auto info() const -> output::string_t
			{
				const auto supported_extensions = vulkan_extension_module::supported_extensions();
				const auto required_extensions = vulkan_extension_module::required_extensions();

				const auto supported =
					std::accumulate(supported_extensions.begin(),
									supported_extensions.end(),
									output::string_t {"\n======== supported " + extension_type() + ": ========\n"},
									[this](const auto& accumulator, const auto& extension) {
										return std::move(accumulator) + '\t' + extension_name(extension) + '\n';
									});

				const auto required = std::accumulate(required_extensions.begin(),
													  required_extensions.end(),
													  output::string_t {"\n======== required " + extension_type() +
																		": ========\n"},
													  [](const auto& accumulator, const auto& extension) {
														  return std::move(accumulator) + '\t' +
																 static_cast<const char*>(extension) + '\n';
													  });

				return supported + required;
			}

		private:
			typedef T::value_type extension_t;

			virtual auto extension_type() const -> output::string_t { return {}; };
			auto extension_name(const extension_t& extension) const -> output::string_t
			{
				if constexpr (std::is_same_v<T, vk_layers_t>)
					return extension.layerName.data();
				else
					return extension.extensionName.data();
			}

			T m_supported_extensions;
			vk_string_t m_required_extensions;
		};

		// ==========================================================================
		// vulkan extension specialization for logical (instance) extensions
		// ==========================================================================
		class logical_extensions : public vulkan_extension_module<vk_extensions_t>
		{
		public:
			using vulkan_extension_module::vulkan_extension_module;

			static inline const auto m_default_logical_extensions = create_info {
				{"VK_EXT_debug_utils", "VK_KHR_get_physical_device_properties2", "VK_KHR_get_surface_capabilities2"}};

			logical_extensions(const vk_extensions_t& supported,
							   const create_info& create_info = m_default_logical_extensions);

		private:
			auto extension_type() const -> output::string_t override;
		};

		// ==========================================================================
		// vulkan extension specialization for physical (device) extensions
		// ==========================================================================
		class physical_extensions : public vulkan_extension_module<vk_extensions_t>
		{
		public:
			using vulkan_extension_module::vulkan_extension_module;

			static inline const auto m_default_physical_extensions = create_info {
				{"VK_KHR_swapchain", "VK_EXT_memory_budget", "VK_KHR_portability_subset"}};

			physical_extensions(const vk_extensions_t& supported,
								const create_info& create_info = m_default_physical_extensions);

		private:
			auto extension_type() const -> output::string_t override;
		};

		// ==========================================================================
		// vulkan extension specialization for validation layers
		// ==========================================================================
		class validation_layers : public vulkan_extension_module<vk_layers_t>
		{
		public:
			static inline const auto m_default_validation_layers = create_info {{"VK_LAYER_KHRONOS_validation",
																				 "VK_LAYER_NV_optimus",
																				 "VK_LAYER_KHRONOS_synchronization2",
																				 "VK_LAYER_LUNARG_monitor",
																				 "VK_LAYER_KHRONOS_profiles"}};

			validation_layers(const vk_layers_t& supported,
							  const create_info& create_info = m_default_validation_layers);

		private:
			auto extension_type() const -> output::string_t override;
		};

		// ==========================================================================
		// ==========================================================================
		struct debug_messanger : public vk_wrapper<vk::raii::DebugUtilsMessengerEXT>
		{
			using vk_wrapper::vk_wrapper;
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
			};

			debug_messanger(const vk::raii::Instance&, const create_info& = {});

		private:
			static VKAPI_ATTR auto VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT,
															 VkDebugUtilsMessageTypeFlagsEXT,
															 const VkDebugUtilsMessengerCallbackDataEXT*,
															 void* user_data) -> VkBool32;
		};

		// ==========================================================================
		// ==========================================================================
		struct instance : public vk_wrapper<vk::raii::Instance>
		{
			struct create_info
			{
				version m_engine_version {version::m_engine_version};
				version m_vulkan_version {version::m_vulkan_version};

				vk_string_t m_required_extensions {
					logical_extensions::m_default_logical_extensions.m_required_extensions};
				vk_string_t m_required_validation_layers {
					validation_layers::m_default_validation_layers.m_required_extensions};

				bool m_using_validation {true};
			};

			instance(const window&, const create_info& = {});
			auto info() const -> output::string_t override;

			std::optional<std::pair<validation_layers, debug_messanger>> m_validation;
			logical_extensions m_extensions;
			vk::raii::Context m_context;
			version m_vulkan_version;
		};

		// ==========================================================================
		// ==========================================================================
		struct physical_device : public vulkan::vk_wrapper<vk::raii::PhysicalDevice>
		{
			using performance_score_t = uint64_t;

			struct create_info
			{
				physical_extensions::create_info m_physical_extensions_info {
					physical_extensions::m_default_physical_extensions};
				performance_score_t m_minimum_accepted_score {0xFFFFFFFF};
			};

			physical_device(const instance&, const create_info& = {});

			static auto get_performance_score(const vk::raii::PhysicalDevice&) -> performance_score_t;

			physical_extensions m_physical_extensions;
		};

		// ==========================================================================
		// ==========================================================================
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
	}
}
