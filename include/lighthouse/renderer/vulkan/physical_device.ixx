module;
#pragma once

export module physical_device;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/renderer/vulkan/instance.ixx"
#include "lighthouse/renderer/vulkan/extension.ixx"
#include "lighthouse/string/string.ixx"
#else
import lighthouse_string;
import extension;
import instance;
import raii_wrapper;
#endif

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
																VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME}};

				performance_score_t m_minimum_accepted_score {0xFFFFFFFF};
			};

			struct physical_properties
			{
				vk::PhysicalDeviceProperties2 m_properties {};

				vk::PhysicalDeviceDescriptorBufferPropertiesEXT m_descriptor_buffer_properties {};
				vk::PhysicalDeviceShaderObjectPropertiesEXT m_shader_object_properties {};
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
