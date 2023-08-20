module;

#include "vkfw/vkfw.hpp"

module instance;
import output;

namespace lh
{
	namespace vulkan
	{

		instance::instance(const window& window, const create_info& create_info)
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

			const auto application_info = vk::ApplicationInfo {window.title().data(),
															   create_info.m_engine_version,
															   window.title().data(),
															   create_info.m_engine_version,
															   create_info.m_vulkan_version};
			if (using_validation)
			{
				m_validation.emplace(
					std::make_pair(vulkan::validation_layers {vk::enumerateInstanceLayerProperties(),
															  {create_info.m_validation_layers.m_required_extensions}},
								   debug_messanger {vk::raii::DebugUtilsMessengerEXT {nullptr}}));

				if (!m_validation->first.assert_required_extensions())
					output::error() << "this system does not support the required vulkan validation layers";
			}

			if (!m_extensions.assert_required_extensions())
				output::error() << "this system does not support the required vulkan logical extensions";

			const auto extensions = m_extensions.required_extensions();
			const auto validation_layers = using_validation ? m_validation->first.required_extensions()
															: vk_string_t {};
			const auto instance_debugger = using_validation ? &create_info.m_debug_messanger.m_debug_info : nullptr;

			const auto instance_info =
				vk::InstanceCreateInfo {{}, &application_info, validation_layers, extensions, instance_debugger};

			m_object = {m_context, instance_info};

			if (using_validation)
				m_validation->second = {m_object};
		}

		auto instance::info() const -> string::string_t
		{
			return {};
		}

		auto instance::validation_layers() const -> std::optional<vulkan::validation_layers>
		{
			if (m_validation.has_value())
				return m_validation->first;

			return std::nullopt;
		}

		auto instance::extensions() const -> logical_extensions
		{
			return m_extensions;
		}

		auto instance::version() const -> lh::version
		{
			return m_vulkan_version;
		}
	}
}
