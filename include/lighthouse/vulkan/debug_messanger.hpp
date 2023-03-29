#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"
#include "lighthouse/output.hpp"

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
		};
	}
}