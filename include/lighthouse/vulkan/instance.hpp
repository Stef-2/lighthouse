#pragma once

#include "lighthouse/vulkan/extension.hpp"
#include "lighthouse/vulkan/raii_wrapper.hpp"
#include "lighthouse/version.hpp"
#include "lighthouse/vulkan/debug_messanger.hpp"

namespace lh
{
	// forward declarations
	class window;

	namespace vulkan
	{
		struct instance : public vk_wrapper<vk::raii::Instance>
		{
			struct create_info
			{
				version m_engine_version = version::m_engine_version;
				version m_vulkan_version = version::m_vulkan_version;
				logical_extensions::create_info m_extensions = logical_extensions::m_default_logical_extensions;
				validation_layers::create_info m_validation_layers = validation_layers::m_default_validation_layers;
				debug_messanger::create_info m_debug_messanger = {};
			};

			instance(const lh::window&, const create_info& = {});
			auto info() const -> lh::string::string_t override;

			std::optional<std::pair<validation_layers, debug_messanger>> m_validation;
			logical_extensions m_extensions;
			vk::raii::Context m_context;
			version m_vulkan_version;
		};
	}
}
