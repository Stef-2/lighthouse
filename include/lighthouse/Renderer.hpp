#pragma once

#include "vkfw.hpp"
#include "vulkan.hpp"
#include "vulkan_raii.hpp"

#include "output.hpp"
#include "window.hpp"
#include "static.hpp"
#include "engine.hpp"

#include <vector>

namespace lh
{
	struct vulkan_version
	{
		uint8_t m_major;
		uint8_t m_minor;
		uint8_t m_patch;
	};

	class renderer
	{
	public:
		friend class engine;

		// enumerate implementation supported extensions and validation layers
		static auto supported_extensions() -> std::vector<vk::ExtensionProperties>;
		static auto supported_validation_layers() -> std::vector<vk::LayerProperties>;

		// enumerate required extensions and validation layers
		static auto required_extensions() -> std::vector<const char*>;
		static auto required_validation_layers() -> std::vector<const char*>;

	private:
		static auto initialize(window&, const vulkan_version&) -> void;

		// check if the system supports required extensions and validation layers
		static auto check_required_extensions() -> bool;
		static auto check_required_validation_layers() -> bool;

		static inline auto m_context = vk::raii::Context{};
		static inline auto m_instance = vk::Instance {nullptr};
		static inline auto m_surface = vk::SurfaceKHR {nullptr};

		static const inline auto m_using_validation_layers = true;

		static const inline auto m_required_extensions = std::vector<const char*> {"VK_EXT_debug_utils"};
		static const inline auto m_required_validation_layers = std::vector<const char*> {"VK_LAYER_KHRONOS_validation"};
	};
}
