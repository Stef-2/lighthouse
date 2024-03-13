module;

#if INTELLISENSE
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/mat4x4.hpp"

#include <concepts>
#endif

export module geometry;

#if not INTELLISENSE
import glm;
#endif

export namespace lh
{
	namespace geometry
	{
		using scalar_t = float;
		
		using position_t = glm::vec3;
		using normal_t = glm::vec3;
		using orientation_t = glm::quat;
		using quaternion_t = glm::qua<scalar_t>;
		using scale_t = glm::vec3;
		using transformation_t = glm::mat4x4;

		constexpr auto epsilon = scalar_t {1.0e-6};

		struct direction_t : public quaternion_t
		{
			using quaternion_t::quaternion_t;

			auto rotate(const quaternion_t& value) { *this *= value; }
			auto rotate(const glm::vec3& value) { *this *= quaternion_t {value}; }

			auto euler() const { return glm::eulerAngles(*this); }

			operator quaternion_t&() { return *this; }
			operator const quaternion_t&() const { return *this; }
			operator const normal_t() { return euler(); }
		};

		struct line
		{
			position_t m_x;
			position_t m_y;
		};

		struct triangle
		{
			position_t m_x;
			position_t m_y;
			position_t m_z;
		};

		struct sphere
		{
			position_t m_position;
			scalar_t m_radius;
		};

		struct aabb
		{
			position_t m_minima;
			position_t m_maxima;
		};

		struct plane
		{
			normal_t m_normal;
			scalar_t m_distance;
		};

		struct ray
		{
			position_t m_position;
			normal_t m_direction;
		};
	}
}