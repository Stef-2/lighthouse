module;
#pragma once

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

export module camera;

#if INTELLISENSE
#include "lighthouse/entity.ixx"
#else
import entity;
#endif

export namespace lh
{
	enum class camera_type
	{
		perspective,
		orthographic
	};

	template <camera_type T = camera_type::perspective>
	class camera : public entity
	{
	public:
		
		template <camera_type T = camera_type::perspective>
		struct create_info
		{
			float m_aspect_ratio = 1080.0f / 1920.0f;
			float m_near_clip = 0.1f;
			float m_far_clip = 1024.0f;
			float m_field_of_view = 90.0f;
		};

		template <>
		struct create_info<camera_type::orthographic>
		{
			float m_left {};
			float m_right {};
			float m_bottom {};
			float m_top {};
			float m_near_clip = 0.1f;
			float m_far_clip = 1024.0f;
		};

		camera(const node& = {}, const create_info<T>& = {});

		static auto up_direction() -> const glm::vec3&;

		auto look_at(const glm::vec3& target) -> void
		{
			rotate_absolute(glm::lookAt(m_position, target, camera::s_up_direction));
		}

		auto view() const -> const entity::transformation_t&
		{
			return local_transformation();
		}

		auto properties(const create_info<T>&) -> void;
		auto properties() const -> const create_info<T>&;

		auto perspective() const -> const glm::mat4x4&;

	private:
		static inline const auto s_up_direction = glm::vec3 {0.0f, 1.0f, 0.0f};

		create_info<T> m_camera_info;
		glm::mat4x4 m_perspective;
	};
}