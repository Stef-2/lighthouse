module;

module camera;

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
		  m_first_person_callback {[this](input::mouse::move_info move_info) -> void {
			  const auto delta_x = static_cast<float>(move_info.m_current.x - move_info.m_previous.x);
			  const auto delta_y = static_cast<float>(move_info.m_current.y - move_info.m_previous.y);

			  const auto quat_delta_x = (glm::ext::angleAxis(glm::radians(delta_y), glm::vec3(1.0f, 0.0f, 0.0f)));
			  const auto quat_delta_y = (glm::ext::angleAxis(glm::radians(delta_x), glm::vec3(0.0f, 1.0f, 0.0f)));

			  rotate_absolute(glm::quat {1.0f, 0.0f, 0.0f, 0.0f} * quat_delta_x * m_orientation * quat_delta_y);
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
		  m_first_person_callback {[this](input::mouse::move_info) -> void {}}
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
