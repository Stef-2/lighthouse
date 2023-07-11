#pragma once

#include "lighthouse/renderer/vulkan/raii_wrapper.hpp"

#if INTELLISENSE
#include "lighthouse/output.ixx"
#else
import output;
#endif

namespace lh
{
	namespace vulkan
	{
		class debug_messanger : public vk_wrapper<vk::raii::DebugUtilsMessengerEXT>
		{
		public:
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

			static inline constexpr auto m_abort_on_error = false;
			static inline constexpr auto m_only_report_once = true;

			static inline auto m_previously_reported_messages =
				std::vector<decltype(vk::DebugUtilsMessengerCallbackDataEXT::messageIdNumber)> {};
		};
	}
}
