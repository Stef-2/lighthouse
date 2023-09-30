module;

#include "glm/vec3.hpp"

export module geometry;

export namespace lh
{
	namespace geometry
	{
		using point_t = glm::vec3;
		using vector_t = glm::vec3;
		using scalar_t = double;

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