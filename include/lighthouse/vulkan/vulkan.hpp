#pragma once

#include "vulkan/glslang/SPIRV/GlslangToSpv.h"

#include "vulkan/vma/vk_mem_alloc.hpp"
#include "vulkan/utils/geometries.hpp"
#include "vulkan/utils/math.hpp"
// #include "vulkan/utils/raii/raii_shaders.hpp"
#include "vulkan/utils/raii/raii_utils.hpp"

#include "lighthouse/vulkan/raii_wrapper.hpp"
#include "lighthouse/vulkan/extension.hpp"
#include "lighthouse/vulkan/instance.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/surface.hpp"
#include "lighthouse/vulkan/queue_families.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"
#include "lighthouse/vulkan/command_control.hpp"
#include "lighthouse/vulkan/queue.hpp"
#include "lighthouse/vulkan/image.hpp"
#include "lighthouse/vulkan/swapchain.hpp"
#include "lighthouse/vulkan/shader_module.hpp"
#include "lighthouse/vulkan/descriptor_set_layout.hpp"
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
	}
}
