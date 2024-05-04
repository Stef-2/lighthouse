module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#include "glm/mat4x4.hpp"
#endif

export module push_constant_format;

import geometry;
import data_type;

#if not INTELLISENSE
import glm;
import vulkan_hpp;
#endif

export namespace lh
{
	namespace vulkan
	{
		struct push_constant
		{
			geometry::transformation_t m_mvp;
			geometry::position_t m_view_position;
			geometry::normal_t m_view_direction;
			vk::Extent2D m_window_extent;
			float32_t m_aspect_ratio;
			float32_t m_near_clip;
			float32_t m_far_clip;
			float64_t m_time;
		};
	}
}