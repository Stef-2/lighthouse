module;

#include "vulkan/vulkan_core.h"

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module physical_device;

#if not INTELLISENSE
import vulkan_hpp;
#endif

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
				physical_extensions::create_info m_extensions {
					{VK_KHR_SWAPCHAIN_EXTENSION_NAME,
					 //VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME,
					 VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
					 VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
					 VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
					 VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
					 VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
					 VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
					 VK_KHR_MAINTENANCE_6_EXTENSION_NAME,
					 //"VK_EXT_map_memory_placed", /*VK_EXT_MAP_MEMORY_PLACED_EXTENSION_NAME*/
					 VK_KHR_INDEX_TYPE_UINT8_EXTENSION_NAME}};

				performance_score_t m_minimum_accepted_score {0xFFFFFFFF};
			};

			struct physical_properties
			{
				struct descriptor_buffer
				{
					vk::PhysicalDeviceDescriptorBufferPropertiesEXT m_properties {};
					std::uint16_t m_uniform_buffer_size {};
					std::uint16_t m_uniform_buffer_offset {};
					std::uint16_t m_storage_buffer_size {};
					std::uint16_t m_storage_buffer_offset {};
					std::uint16_t m_combined_image_sampler_size {};
					std::uint16_t m_combined_image_sampler_offset {};
				};

				vk::PhysicalDeviceProperties2 m_properties {};
				vk::PhysicalDeviceShaderObjectPropertiesEXT m_shader_object_properties {};
				vk::PhysicalDeviceDescriptorIndexingProperties m_descriptor_indexing_properties {};
				vk::PhysicalDeviceMaintenance5PropertiesKHR m_maintenance_t_properties {};
				descriptor_buffer m_descriptor_buffer_properties;
			};

			struct physical_features
			{
				vk::PhysicalDeviceFeatures2 m_features {};
				vk::PhysicalDeviceDynamicRenderingFeatures m_dynamic_rendering_features;
				vk::PhysicalDeviceShaderObjectFeaturesEXT m_shader_object_features {};
				vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT m_vertex_dynamic_state_features {};
				vk::PhysicalDeviceDescriptorIndexingFeatures m_descriptor_indexing_features {};
				vk::PhysicalDeviceDescriptorBufferFeaturesEXT m_descriptor_buffer_features {};
				vk::PhysicalDeviceMaintenance5FeaturesKHR m_maintenance_5_features {};
				vk::PhysicalDeviceMaintenance6FeaturesKHR m_maintenance_6_features {};
			};

			physical_device(const instance&, const create_info& = {});

			auto extensions() const -> physical_extensions;
			auto performance_score() const -> performance_score_t;
			auto properties() const -> const physical_properties&;
			auto features() const -> const physical_features&;

			auto info() const -> lh::string::string_t override;

		private:
			static auto performance_score(const vk::raii::PhysicalDevice&) -> performance_score_t;
			static auto preferred_device(const instance&, const create_info&) -> vk::raii::PhysicalDevice;

			physical_extensions m_extensions;
			performance_score_t m_performance_score;
			physical_properties m_properties;
			physical_features m_features;
		};
	}
}