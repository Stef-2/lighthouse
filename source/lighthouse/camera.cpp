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
	camera<camera_type::perspective>::camera(std::shared_ptr<node> node,
											 const create_info<camera_type::perspective>& create_info)
		: entity {node},
		  m_camera_info {create_info},
		  m_projection {glm::perspectiveRH_ZO(glm::radians(m_camera_info.m_field_of_view),
											  m_camera_info.m_aspect_ratio,
											  m_camera_info.m_near_clip,
											  m_camera_info.m_far_clip)},
		  m_first_person_callback {[this](input::mouse::move_data move_data) -> void {
			  const auto delta_x = static_cast<float>(move_data.m_current_x - move_data.m_previous_x);
			  const auto delta_y = static_cast<float>(move_data.m_current_y - move_data.m_previous_y);

			  const auto qX = (glm::angleAxis(glm::radians(delta_y), glm::vec3(1.0f, 0.0f, 0.0f)));
			  const auto qY = (glm::angleAxis(glm::radians(delta_x), glm::vec3(0.0f, 1.0f, 0.0f)));

			  rotate_absolute(glm::quat {1.0f, 0.0f, 0.0f, 0.0f} * qX * m_rotation * qY);
		  }}
	{}

	template <>
	camera<camera_type::orthographic>::camera(std::shared_ptr<node> node,
											  const create_info<camera_type::orthographic>& create_info)
		: entity {node},
		  m_camera_info {create_info},
		  m_projection {glm::orthoZO(m_camera_info.m_left,
									 m_camera_info.m_right,
									 m_camera_info.m_bottom,
									 m_camera_info.m_top,
									 m_camera_info.m_near_clip,
									 m_camera_info.m_far_clip)},
		  m_first_person_callback {[this](input::mouse::move_data) -> void {}}
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
			m_projection = glm::perspectiveRH_ZO(glm::radians(m_camera_info.m_field_of_view),
												 m_camera_info.m_aspect_ratio,
												 m_camera_info.m_near_clip,
												 m_camera_info.m_far_clip);

		if constexpr (std::same_as<T, camera_type::orthographic>)
			m_projection = glm::orthoZO(m_camera_info.m_left,
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
}
