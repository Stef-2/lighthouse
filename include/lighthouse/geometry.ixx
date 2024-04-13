module;

#if INTELLISENSE
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/mat4x4.hpp"

#include <concepts>
#include <numbers>
#include <limits>
#include <array>
#include <optional>
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

		std::optional<position_t> ray_tri_test(const ray& ray, const triangle& triangle)
		{
			auto result = std::optional<position_t> {};

			const auto edge_1 = triangle.m_y - triangle.m_x;
			const auto edge_2 = triangle.m_z - triangle.m_x;
			const auto cross_1 = glm::cross(ray.m_direction, edge_2);
			const float determinant = glm::dot(edge_1, cross_1);

			if (determinant > -epsilon and determinant < epsilon) return result;

			const auto inverse_determinant = scalar_t {1.0} / determinant;
			const auto s = ray.m_position - triangle.m_x;
			const auto u = inverse_determinant * glm::dot(s, cross_1);

			if (u < 0 || u > 1) return result;

			const auto cross_2 = glm::cross(s, edge_1);
			float v = inverse_determinant * glm::dot(ray.m_direction, cross_2);

			if (v < 0 || u + v > 1) return result;

			const auto t = inverse_determinant * glm::dot(edge_2, cross_2);

			if (t > epsilon) result = ray.m_position + ray.m_direction * static_cast<scalar_t>(t);

			return result;
		}

		bool ray_aabb_test(const ray& ray, const aabb& aabb)
		{
			double tmin = -std::numeric_limits<double>::infinity(), tmax = std::numeric_limits<double>::infinity();

			if (ray.m_direction.x != 0.0)
			{
				double tx1 = (aabb.m_minima.x - ray.m_position.x) / ray.m_direction.x;
				double tx2 = (aabb.m_maxima.x - ray.m_position.x) / ray.m_direction.x;

				tmin = std::max(tmin, std::min(tx1, tx2));
				tmax = std::min(tmax, std::max(tx1, tx2));
			}

			if (ray.m_direction.y != 0.0)
			{
				double ty1 = (aabb.m_minima.y - ray.m_position.y) / ray.m_direction.y;
				double ty2 = (aabb.m_maxima.y - ray.m_position.y) / ray.m_direction.y;

				tmin = std::max(tmin, std::min(ty1, ty2));
				tmax = std::min(tmax, std::max(ty1, ty2));
			}

			return tmax >= tmin;
		}
	}
}