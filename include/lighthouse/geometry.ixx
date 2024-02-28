module;

#if INTELLISENSE
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/mat4x4.hpp"
#endif

export module geometry;

#if not INTELLISENSE
import glm;
#endif

export namespace lh
{
	namespace geometry
	{
		using point_t = glm::vec3;
		using vector_t = glm::vec3;
		using direction_t = glm::vec3;
		using scalar_t = float;

		using position_t = glm::vec3;
		using rotation_t = glm::vec3;
		using orientation_t = glm::quat;
		using scale_t = glm::vec3;
		using transformation_t = glm::mat4x4;

		constexpr auto epsilon = 0.00001;

		struct line
		{
			point_t m_x;
			point_t m_y;
		};

		struct triangle
		{
			point_t m_x;
			point_t m_y;
			point_t m_z;
		};

		struct sphere
		{
			point_t m_position;
			scalar_t m_radius;
		};

		struct aabb
		{
			point_t m_minima;
			point_t m_maxima;
		};

		struct plane
		{
			vector_t m_normal;
			scalar_t m_distance;
		};

		struct ray
		{
			point_t m_position;
			vector_t m_direction;
		};
	}
}