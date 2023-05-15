#pragma once

#include "lighthouse/string/string.hpp"
#include "lighthouse/vulkan/extension.hpp"
#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class instance;

		class physical_device : public vk_wrapper<vk::raii::PhysicalDevice>
		{
		public:
			using performance_score_t = uint64_t;

			struct create_info
			{
				physical_extensions::create_info m_extensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME,
																  VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
																  VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
																  VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
																  VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
																  VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
																  VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME}};
				performance_score_t m_minimum_accepted_score {0xFFFFFFFF};
			};

			physical_device(const instance&, const create_info& = {});

			auto extensions() const -> physical_extensions;
			auto performance_score() const -> performance_score_t;
			auto info() const -> lh::string::string_t override;

		private:
			static auto performance_score(const vk::raii::PhysicalDevice&) -> performance_score_t;
			static auto preferred_device(const instance&, const create_info&) -> vk::raii::PhysicalDevice;

			physical_extensions m_extensions;
			performance_score_t m_performance_score;
		};
	}
}
