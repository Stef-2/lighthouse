module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#include "glm/mat4x4.hpp"
#endif

export module push_constant;

import geometry;
import data_type;

#if not INTELLISENSE
import glm;
import vulkan_hpp;
#endif

import std;

export namespace lh
{
	namespace vulkan
	{
		struct push_constant
		{
			struct view_data
			{
				geometry::vec4_t m_view_position {};
				geometry::vec4_t m_view_direction {};
			};

			struct projection_data
			{
				vk::Extent2D m_window_extent {};
				float32_t m_aspect_ratio {};
				float32_t m_near_clip {};
				float32_t m_far_clip {};
			};

			struct buffer_device_address_data
			{
				vk::DeviceAddress m_address_1 {};
				vk::DeviceAddress m_address_2 {};
				vk::DeviceAddress m_address_3 {};
				vk::DeviceAddress m_address_4 {};
				vk::DeviceAddress m_address_5 {};
				vk::DeviceAddress m_address_6 {};
				vk::DeviceAddress m_address_7 {};
				vk::DeviceAddress m_address_8 {};
			};

			geometry::transformation_t m_model {1.0f};
			view_data m_view_data {};
			projection_data m_projection_data {};
			buffer_device_address_data m_registers {};
			float64_t m_current_time {};
			float64_t m_delta_time {};
		};

		static_assert(sizeof push_constant <= 256);
	}
}