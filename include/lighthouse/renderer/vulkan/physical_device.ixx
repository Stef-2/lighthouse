module;

#include "vulkan/vulkan_core.h"

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module physical_device;

import lighthouse_string;
import extension;
import instance;
import raii_wrapper;
import version;

export namespace lh
{
	namespace vulkan
	{
		class physical_device : public raii_wrapper<vk::raii::PhysicalDevice>
		{
		public:
			using performance_score_t = uint64_t;

			struct create_info
			{
				physical_extensions::create_info m_extensions {{VK_KHR_SWAPCHAIN_EXTENSION_NAME,
																VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
																VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
																VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
																VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
																VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
																VK_EXT_HOST_IMAGE_COPY_EXTENSION_NAME}};

				performance_score_t m_minimum_accepted_score {0xFFFFFFFF};
			};

			struct physical_properties
			{
				vk::PhysicalDeviceProperties2 m_properties {};

				vk::PhysicalDeviceDescriptorBufferPropertiesEXT m_descriptor_buffer_properties {};
				vk::PhysicalDeviceShaderObjectPropertiesEXT m_shader_object_properties {};
				vk::PhysicalDeviceHostImageCopyPropertiesEXT m_host_image_copy_properties {};
			};

			physical_device(const instance&, const create_info& = {});

			auto extensions() const -> physical_extensions;
			auto performance_score() const -> performance_score_t;
			auto properties() const -> const physical_properties&;

			auto info() const -> lh::string::string_t override;

		private:
			static auto performance_score(const vk::raii::PhysicalDevice&) -> performance_score_t;
			static auto preferred_device(const instance&, const create_info&) -> vk::raii::PhysicalDevice;

			physical_extensions m_extensions;
			performance_score_t m_performance_score;
			physical_properties m_properties;
		};
	}
}
