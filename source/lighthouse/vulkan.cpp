#include "vulkan.hpp"

VKAPI_ATTR auto VKAPI_CALL
lh::vulkan::debug_messanger::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
											VkDebugUtilsMessageTypeFlagsEXT message_type,
											const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
											void* user_data) -> VkBool32
{
	auto message = output::string_t {};

	message += vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(message_severity)) + ": " +
			   vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(message_type)) + ":\n";
	message += std::string("\t") + "messageIDName   = <" + callback_data->pMessageIdName + ">\n";
	message += std::string("\t") + "messageIdNumber = " + std::to_string(callback_data->messageIdNumber) + "\n";
	message += std::string("\t") + "message         = <" + callback_data->pMessage + ">\n";

	if (callback_data->queueLabelCount > 0)
	{
		message += std::string("\t") + "Queue Labels:\n";

		for (uint32_t i = 0; i < callback_data->queueLabelCount; i++)
			message += std::string("\t\t") + "labelName = <" + callback_data->pQueueLabels[i].pLabelName + ">\n";
	}
	if (callback_data->cmdBufLabelCount > 0)
	{
		message += std::string("\t") + "CommandBuffer Labels:\n";

		for (uint32_t i = 0; i < callback_data->cmdBufLabelCount; i++)
			message += std::string("\t\t") + "labelName = <" + callback_data->pCmdBufLabels[i].pLabelName + ">\n";
	}
	if (callback_data->objectCount > 0)
	{
		for (uint32_t i = 0; i < callback_data->objectCount; i++)
		{
			message += std::string("\t") + "Object " + std::to_string(i) + "\n";
			message += std::string("\t\t") + "objectType = " +
					   vk::to_string(static_cast<vk::ObjectType>(callback_data->pObjects[i].objectType)) + "\n";
			message += std::string("\t\t") +
					   "objectHandle = " + std::to_string(callback_data->pObjects[i].objectHandle) + "\n";
			if (callback_data->pObjects[i].pObjectName)
				message += std::string("\t\t") + "objectName = <" + callback_data->pObjects[i].pObjectName + ">\n";
		}
	}

	switch (message_severity)
	{
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		output::log() << message;
		break;
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		output::warning() << message;
		break;
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		output::error() << message;
		break;
	default: break;
	}

	return false;
}

lh::vulkan::debug_messanger::debug_messanger(const vk::raii::Instance& instance, const create_info& create_info)
	: vk_wrapper((instance.createDebugUtilsMessengerEXT(create_info.m_debug_info)))
{}

lh::vulkan::instance::instance(const window& window, const create_info& create_info)
	: m_context {},
	  m_vulkan_version(create_info.m_vulkan_version),
	  m_extensions(vk::enumerateInstanceExtensionProperties())
{}

auto lh::vulkan::instance::info() const -> output::string_t
{
	return {};
}

lh::vulkan::logical_extensions::logical_extensions(const vk_extensions_t& supported, const create_info& create_info)
	: vulkan_extension_module(supported, create_info)
{}

auto lh::vulkan::logical_extensions::extension_type() const -> output::string_t
{
	return "logical extension";
}

lh::vulkan::physical_extensions::physical_extensions(const vk_extensions_t& supported, const create_info& create_info)
	: vulkan_extension_module(supported, create_info)
{}

auto lh::vulkan::physical_extensions::extension_type() const -> output::string_t
{
	return "physical extension";
}

lh::vulkan::validation_layers::validation_layers(const vk_layers_t& supported,
												 const create_info& create_info /*= m_default_validation_layers*/)
	: vulkan_extension_module(supported, create_info)
{}

auto lh::vulkan::validation_layers::extension_type() const -> output::string_t
{
	return "validation layer";
}
