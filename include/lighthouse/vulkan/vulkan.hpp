#pragma once

#include "vulkan/glslang/SPIRV/GlslangToSpv.h"
// #include "vkfw/vkfw.hpp"
#include "vulkan/vma/vk_mem_alloc.hpp"
#include "vulkan/utils/geometries.hpp"
#include "vulkan/utils/math.hpp"
#include "vulkan/utils/raii/raii_shaders.hpp"
#include "vulkan/utils/raii/raii_utils.hpp"
// #include "vulkan/vulkan.hpp"
// #include "vulkan/vulkan_raii.hpp"
// #include "vulkan/vulkan_to_string.hpp"

#include "lighthouse/vulkan/raii_wrapper.hpp"
#include "lighthouse/vulkan/extension.hpp"
#include "lighthouse/vulkan/instance.hpp"
#include "lighthouse/window.hpp"
#include "lighthouse/version.hpp"
#include "lighthouse/memory.hpp"

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
		// ==========================================================================

		// ==========================================================================
		// ==========================================================================

		// ==========================================================================
		// ==========================================================================
		/*
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
			auto info() const -> lh::output::string_t override;

			static auto performance_score(const vk::raii::PhysicalDevice&) -> performance_score_t;

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
			};

			logical_device(const physical_device&,
						   const std::vector<vk::DeviceQueueCreateInfo>&,
						   const vulkan::vk_string_t&,
						   const create_info& = {});
		};*/
	}
}
