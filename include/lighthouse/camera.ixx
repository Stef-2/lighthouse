module;

#if INTELLISENSE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <functional>
#include <memory>
#endif

#include "glm/mat4x4.hpp"
#include "glm/gtx/quaternion.hpp"

export module camera;

import entity;
import input;

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
			float m_aspect_ratio = 1920.0f / 1080.0f;
			float m_near_clip = 0.1f;
			float m_far_clip = 1024.0f;
			float m_field_of_view = 45.0f;
		};

		template <>
		struct create_info<camera_type::orthographic>
		{
			float m_left;
			float m_right;
			float m_bottom;
			float m_top;
			float m_near_clip;
			float m_far_clip;
		};

		camera(std::shared_ptr<node>, const create_info<T>& = {});

		static auto up_direction() -> const glm::vec3&
		{
			return s_up_direction;
		}

		auto view() const -> const node::transformation_t&
		{
			return local_transformation();
		}

		auto view_direction() const -> const glm::vec3
		{
			const auto& transformation = local_transformation();

			return {transformation[0][2], transformation[1][2], transformation[2][2]};
		}

		auto right_direction() const -> const glm::vec3
		{
			return glm::cross(view_direction(), s_up_direction);
		}

		auto look_at(const entity::vector3_t& target) -> void
		{
			auto view = glm::quatLookAt(glm::normalize(m_position + target), s_up_direction);
			// don't ask
			view.w *= -1.0f;

			rotate_absolute(view);
		}

		auto properties(const create_info<T>&) -> void;
		auto properties() const -> const create_info<T>&;

		auto projection() const -> const glm::mat4x4&
		{
			return m_projection;
		}
		
		auto first_person_callback() const -> const input::mouse::on_move_action_t&
		{
			return m_first_person_callback;
		}
		
	private:

		static inline const auto s_up_direction = glm::vec3 {0.0f, 1.0f, 0.0f};

		create_info<T> m_camera_info;
		glm::mat4x4 m_projection;

		input::mouse::on_move_action_t m_first_person_callback;
	};
}