module;

#if INTELLISENSE
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/mat4x4.hpp"
#endif

export module geometry;

import data_type;
import math;
import static_math;

#if not INTELLISENSE
import glm;
#endif

import std;

export namespace lh
{
	namespace geometry
	{
		using scalar_t = lh::float32_t;
		constexpr auto epsilon = std::numeric_limits<scalar_t>::epsilon();

		using vec3_t = glm::vec<3, scalar_t>;
		using vec4_t = glm::vec<4, scalar_t>;
		using position_t = glm::vec<3, scalar_t>;
		using normal_t = glm::vec<3, scalar_t>;
		using quaternion_t = glm::qua<scalar_t>;
		using scale_t = glm::vec<3, scalar_t>;
		using transformation_t = glm::mat<4, 4, scalar_t>;

		struct rotation_t : public quaternion_t
		{
			using quaternion_t::quaternion_t;

			rotation_t();
			rotation_t(scalar_t, scalar_t, scalar_t);
			rotation_t(const quaternion_t&);
			rotation_t(const normal_t&);

			auto operator=(const quaternion_t& value) -> void;
			auto operator=(const normal_t& value) -> void;

			auto rotate(const quaternion_t&) -> void;
			auto rotate(const normal_t&) -> void;

			auto euler_degrees_cast() const -> const normal_t;
			auto euler_radians_cast() const -> const normal_t;
			auto matrix_cast() const -> const transformation_t;

			auto dot_product(const rotation_t& other) const -> const scalar_t;
			auto cross_product(const rotation_t& other) const -> const quaternion_t;

			operator quaternion_t&();
			operator const quaternion_t&() const;
			operator normal_t();
			operator const normal_t();
			operator transformation_t();
			operator const transformation_t() const;
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
			auto center() const -> const position_t;
			auto transformation() const -> const transformation_t;

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