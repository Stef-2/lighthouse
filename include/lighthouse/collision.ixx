module;

#if INTELLISENSE
#include "glm/glm.hpp"
#endif

export module collision;

import geometry;

#if not INTELLISENSE
import glm;
#endif

import std;

export namespace lh
{
	namespace collision
	{
		std::optional<lh::geometry::position_t> ray_tri_test(const lh::geometry::ray& ray, const lh::geometry::triangle& triangle)
		{
			auto result = std::optional<lh::geometry::position_t> {};

			const auto edge_1 = triangle.m_y - triangle.m_x;
			const auto edge_2 = triangle.m_z - triangle.m_x;
			const auto cross_1 = glm::cross(ray.m_direction, edge_2);
			const float determinant = glm::dot(edge_1, cross_1);

			if (determinant > -lh::geometry::epsilon and determinant < lh::geometry::epsilon) return result;

			const auto inverse_determinant = lh::geometry::scalar_t {1.0} / determinant;
			const auto s = ray.m_position - triangle.m_x;
			const auto u = inverse_determinant * glm::dot(s, cross_1);

			if (u < 0 || u > 1) return result;

			const auto cross_2 = glm::cross(s, edge_1);
			float v = inverse_determinant * glm::dot(ray.m_direction, cross_2);

			if (v < 0 || u + v > 1) return result;

			const auto t = inverse_determinant * glm::dot(edge_2, cross_2);

			if (t > lh::geometry::epsilon)
				result = ray.m_position + ray.m_direction * static_cast<lh::geometry::scalar_t>(t);

			return result;
		}

		bool ray_aabb_test(const lh::geometry::ray& ray, const lh::geometry::aabb& aabb)
		{
			float tmin = 0.0, tmax = std::numeric_limits<float>::infinity();
			const auto ray_inv = glm::vec3 {1.0f} / ray.m_direction;

			for (int d = 0; d < 3; ++d)
			{
				float t1 = (aabb.m_minima[d] - ray.m_position[d]) * ray_inv[d];
				float t2 = (aabb.m_maxima[d] - ray.m_position[d]) * ray_inv[d];

				tmin = std::min(std::max(t1, tmin), std::max(t2, tmin));
				tmax = std::max(std::min(t1, tmax), std::min(t2, tmax));
			}

			return tmin <= tmax;
		}
	}
}