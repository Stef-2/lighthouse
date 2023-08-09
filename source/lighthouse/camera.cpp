module;

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#if INTELLISENSE
#include "lighthouse/camera.ixx"
#else
module camera;
#endif

namespace lh
{
	template <>
	camera<camera_type::perspective>::camera(const node& node, const create_info<camera_type::perspective>& create_info)
		: entity {node},
		  m_camera_info {create_info},
		  m_perspective {glm::perspective(m_camera_info.m_field_of_view,
										  m_camera_info.m_aspect_ratio,
										  m_camera_info.m_near_clip,
										  m_camera_info.m_far_clip)}
	{}

	template <>
	camera<camera_type::orthographic>::camera(const node& node,
											  const create_info<camera_type::orthographic>& create_info)
		: entity {node},
		  m_camera_info {create_info},
		  m_perspective {glm::orthoZO(m_camera_info.m_left,
									  m_camera_info.m_right,
									  m_camera_info.m_bottom,
									  m_camera_info.m_top,
									  m_camera_info.m_near_clip,
									  m_camera_info.m_far_clip)}
	{}

	template <camera_type T>
	auto camera<T>::up_direction() -> const glm::vec3&
	{
		return camera::s_up_direction;
	}

	template <camera_type T>
	auto camera<T>::properties(const create_info<T>& create_info) -> void
	{
		m_camera_info = create_info;

		if constexpr (std::same_as<T, camera_type::perspective>)
			m_perspective = glm::perspective(m_camera_info.m_field_of_view,
											 m_camera_info.m_aspect_ratio,
											 m_camera_info.m_near_clip,
											 m_camera_info.m_far_clip);

		if constexpr (std::same_as<T, camera_type::orthographic>)
			m_perspective = glm::orthoZO(m_camera_info.m_left,
										 m_camera_info.m_right,
										 m_camera_info.m_bottom,
										 m_camera_info.m_top,
										 m_camera_info.m_near_clip,
										 m_camera_info.m_far_clip);
	}

	template <camera_type T>
	auto camera<T>::properties() const -> const create_info<T>&
	{
		return m_camera_info;
	}

	template <camera_type T>
	auto camera<T>::perspective() const -> const glm::mat4x4&
	{
		return m_perspective;
	}
}
