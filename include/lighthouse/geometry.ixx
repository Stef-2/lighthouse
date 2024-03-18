module;

#if INTELLISENSE
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/mat4x4.hpp"

#include <concepts>
#include <numbers>
#include <limits>
#include <array>
#endif

export module geometry;

#if not INTELLISENSE
import std.core;
import glm;
#endif

import data_type;
import math;
import static_math;

export namespace lh
{
	namespace geometry
	{
		using scalar_t = lh::float32_t;
		constexpr auto epsilon = std::numeric_limits<scalar_t>::epsilon();

		using vec3f_t = glm::vec<3, scalar_t>;
		using position_t = glm::vec<3, scalar_t>;
		using normal_t = glm::vec<3, scalar_t>;
		using quaternion_t = glm::qua<scalar_t>;
		using scale_t = glm::vec<3, scalar_t>;
		using transformation_t = glm::mat<4, 4, scalar_t>;
		/*
		struct position_t : public vec3f_t
		{
			using vec3f_t::vec3f_t;

			position_t() : vec3f_t {0.0f, 0.0f, 0.0f} {}
			//position_t(const vec3f_t& value) : vec3f_t {value} {}
			//position_t(float x, int y, int z) : vec3f_t {x, y, z} {}
			//position_t(int x, int y, float z) : vec3f_t {x, y, z} {}
			//position_t(int x, float y, int z) : vec3f_t {x, y, z} {}

			position_t(auto x, auto y, auto z) : vec3f_t
			{
				static_cast<float>(x), static_cast<float>(y), static_cast<float>(z),
			} {}

			auto operator+=(const vec3f_t& vector) { static_cast<vec3f_t>(*this) += vector; }
			auto operator+=(const position_t& other) { static_cast<vec3f_t>(*this) += static_cast<vec3f_t>(other); }

			operator vec3f_t() { return *this; }
			operator const vec3f_t() const { return *this; }
			operator vec3f_t&() { return *this; }
			operator const vec3f_t&() const { return *this; }
		};*/

		struct direction_t : public quaternion_t
		{
			using quaternion_t::quaternion_t;

			direction_t();
			direction_t(scalar_t x, scalar_t y, scalar_t z) : quaternion_t {{x, y, z}} {}
			direction_t(const quaternion_t&);
			direction_t(const normal_t&);

			auto operator=(const quaternion_t& value) -> void { static_cast<quaternion_t>(*this) = value; }
			auto operator=(const normal_t& value) -> void { static_cast<quaternion_t>(*this) = quaternion_t {value}; }

			auto rotate(const quaternion_t&) -> void;
			auto rotate(const normal_t&) -> void;

			auto euler_degrees_cast() const -> const normal_t;
			auto euler_radians_cast() const -> const normal_t;
			auto matrix_cast() const -> const transformation_t;

			auto dot_product(const direction_t& other) const -> const scalar_t;
			auto cross_product(const direction_t& other) const -> const quaternion_t;

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
