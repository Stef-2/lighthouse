module;

#if INTELLISENSE
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/mat4x4.hpp"

#include <concepts>
#include <numbers>
#include <array>
#endif

export module geometry;

#if not INTELLISENSE
import std.core;
import glm;
#endif

import static_math;

export namespace lh
{
	namespace geometry
	{
		using scalar_t = float;
		constexpr auto epsilon = scalar_t {1.0e-6};

		using position_t = glm::vec<3, scalar_t>;
		using normal_t = glm::vec<3, scalar_t>;
		using orientation_t = glm::quat;
		using quaternion_t = glm::qua<scalar_t>;
		using scale_t = glm::vec<3, scalar_t>;
		using transformation_t = glm::mat<4, 4, scalar_t>;
		
		constexpr scalar_t operator""_to_radians(long double degrees) { return degrees * std::numbers::pi_v<decltype(degrees)> / 180; }
		constexpr scalar_t operator""_to_degrees(long double radians) { return radians * 180 / std::numbers::pi_v<decltype(radians)>; }

		

		constexpr auto omg = lh::math::static_math::sin<double, 30>(30.0_to_radians);
		constexpr auto asg = lh::math::static_math::cos4(1.0/*30.0_to_radians*/);
		constexpr auto asd = lh::math::static_math::pow(2.0f, 60);
		//constexpr auto default_orientation = std::vector
		struct direction_t : public quaternion_t
		{
			using quaternion_t::quaternion_t;

			direction_t() : quaternion_t {} {}

			auto rotate(const quaternion_t& value) { *this *= value; }
			auto rotate(const glm::vec3& value) { *this *= quaternion_t {value}; }

			auto euler_degrees_cast() const { return glm::degrees(glm::eulerAngles(*this)); }
			auto euler_radians_cast() const { return glm::eulerAngles(*this); }
			auto matrix_cast() const { return glm::mat4_cast(*this); }
			/*
			auto dot_product(const direction_t& other) const { return glm::dot(*this, other); }
			auto cross_product(const direction_t& other) const { return glm::cross(*this, other); }
			*/
			/*
			[[nodiscard]] auto conjugate() const { return glm::conjugate(*this); }
			auto conjugate() { *this = glm::conjugate(*this); }
			[[nodiscard]] auto inverse() const { return glm::inverse(*this); }
			auto inverse() { *this = glm::inverse(*this); }
			[[nodiscard]] auto normalize() const { return glm::normalize(*this); }
			auto normalize() { *this = glm::normalize(*this); }
			*/
			operator quaternion_t&() { return *this; }
			operator const quaternion_t&() const { return *this; }
			operator normal_t() { return euler_radians_cast(); }
			operator const normal_t() { return euler_radians_cast(); }
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
