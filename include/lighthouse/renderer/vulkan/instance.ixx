module;

#include "vulkan/vulkan_core.h"

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <utility>
#include <optional>
#endif

export module instance;

import version;
import window;
import raii_wrapper;
import extension;
import debug_messanger;

#if not INTELLISENSE
import std.core;
import vulkan_hpp;
#endif

export namespace lh
{
	namespace vulkan
	{
		class instance : public raii_wrapper<vk::raii::Instance>
		{
		public:
			struct create_info
			{
				lh::version m_engine_version;
				lh::version m_vulkan_version;
				logical_extensions::create_info m_extensions = {
					{VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME}

				};
				validation_layers::create_info m_validation_layers = {{"VK_LAYER_KHRONOS_validation",
																	   "VK_LAYER_NV_optimus",
																	   "VK_LAYER_KHRONOS_synchronization2",
																	   "VK_LAYER_LUNARG_monitor"}};
				#if not INTELLISENSE
				debug_messanger::create_info m_debug_messanger = {};
				#endif
			};

			instance(const lh::window&, const create_info&);

			auto context() const -> const vk::raii::Context&;
			auto info() const -> lh::string::string_t override;
			auto validation_layers() const -> std::optional<vulkan::validation_layers>;
			auto extensions() const -> logical_extensions;
			auto version() const -> lh::version;

		private:
			std::optional<std::pair<vulkan::validation_layers, debug_messanger>> m_validation;
			logical_extensions m_extensions;
			vk::raii::Context m_context;
			lh::version m_vulkan_version;
		};
	}
}