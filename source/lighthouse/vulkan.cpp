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
	  m_extensions(vk::enumerateInstanceExtensionProperties(),
				   {std::accumulate(vkfw::getRequiredInstanceExtensions().begin(),
									vkfw::getRequiredInstanceExtensions().end(),
									create_info.m_extensions.m_required_extensions,
									[](auto base_extensions, const auto& glfw_extensions) {
										base_extensions.emplace_back(glfw_extensions);
										return std::move(base_extensions);
									})})
{
	const auto using_validation = not create_info.m_validation_layers.m_required_extensions.empty();

	const auto application_info = vk::ApplicationInfo {window.get_title().data(),
													   create_info.m_engine_version,
													   window.get_title().data(),
													   create_info.m_engine_version,
													   create_info.m_vulkan_version};
	if (using_validation)
	{
		m_validation.emplace(std::make_pair(validation_layers {vk::enumerateInstanceLayerProperties(),
															   {create_info.m_validation_layers.m_required_extensions}},
											debug_messanger {vk::raii::DebugUtilsMessengerEXT {nullptr}}));

		if (!m_validation->first.assert_required_extensions())
			output::error() << "this system does not support the required vulkan validation layers";
	}

	if (!m_extensions.assert_required_extensions())
		output::error() << "this system does not support the required vulkan logical extensions";

	const auto extensions = m_extensions.required_extensions();
	const auto validation_layers = using_validation ? m_validation->first.required_extensions() : vk_string_t {};
	const auto instance_debugger = using_validation ? &create_info.m_debug_messanger.m_debug_info : nullptr;

	const auto instance_info =
		vk::InstanceCreateInfo {{}, &application_info, validation_layers, extensions, instance_debugger};

	m_object = {m_context, instance_info};

	if (using_validation)
		m_validation->second = {m_object};
}

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

lh::vulkan::validation_layers::validation_layers(const vk_layers_t& supported, const create_info& create_info)
	: vulkan_extension_module(supported, create_info)
{}

auto lh::vulkan::validation_layers::extension_type() const -> output::string_t
{
	return "validation layer";
}
