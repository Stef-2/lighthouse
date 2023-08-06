module;

#include "glm/gtc/matrix_transform.hpp"

#if INTELLISENSE
#include "lighthouse/camera.ixx"
#else
module camera;
#endif

namespace lh
{
	camera::camera(const node& node, const create_info& create_info)
		: entity {node},
		  m_aspect_ratio {create_info.m_aspect_ratio},
		  m_near_clip {create_info.m_near_clip},
		  m_far_clip {create_info.m_far_clip},
		  m_field_of_view {create_info.m_field_of_view}
	{}

	auto camera::view() const -> const entity::transformation_t
	{
		return glm::perspective(m_field_of_view, m_aspect_ratio, m_near_clip, m_far_clip);
	}
}
