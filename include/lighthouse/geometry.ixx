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
		
		struct hit
		{
			float u, v, w, t;
		};

		ray r {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}};

		triangle t {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

		#define BACKFACE_CULLING 0
		hit ray_tri_test(ray ray = r, triangle tri = t)
		{
			hit hit {};

			const auto& dir = ray.m_direction;
			const auto& org = ray.m_position;

			int kz = std::max({abs(ray.m_direction.x), abs(ray.m_direction.y), abs(ray.m_direction.z)});
			int kx = kz + 1;
			if (kx == 3) kx = 0;
			int ky = kx + 1;
			if (ky == 3) ky = 0;

			if (dir[kz] < 0.0f) std::swap(kx, ky);

			float Sx = dir[kx] / dir[kz];
			float Sy = dir[ky] / dir[kz];
			float Sz = 1.0f / dir[kz];

			const glm::vec3 A = tri.m_x - org;
			const glm::vec3 B = tri.m_y - org;
			const glm::vec3 C = tri.m_z - org;

			const float Ax = A[kx] - Sx * A[kz];
			const float Ay = A[ky] - Sy * A[kz];
			const float Bx = B[kx] - Sx * B[kz];
			const float By = B[ky] - Sy * B[kz];
			const float Cx = C[kx] - Sx * C[kz];
			const float Cy = C[ky] - Sy * C[kz];

			float U = Cx * By - Cy * Bx;
			float V = Ax * Cy - Ay * Cx;
			float W = Bx * Ay - By * Ax;

			if (U == 0.0f || V == 0.0f || W == 0.0f)
			{
				double CxBy = (double)Cx * (double)By;
				double CyBx = (double)Cy * (double)Bx;
				U = (float)(CxBy - CyBx);
				double AxCy = (double)Ax * (double)Cy;
				double AyCx = (double)Ay * (double)Cx;
				V = (float)(AxCy - AyCx);
				double BxAy = (double)Bx * (double)Ay;
				double ByAx = (double)By * (double)Ax;
				W = (float)(BxAy - ByAx);
			}

			#ifdef BACKFACE_CULLING
			if (U < 0.0f || V < 0.0f || W < 0.0f) return hit;
			#else
			if ((U < 0.0f || V < 0.0f || W < 0.0f) && (U > 0.0f || V > 0.0f || W > 0.0f)) return;
			#endif

			float det = U + V + W;
			if (det == 0.0f) return hit;

			const float Az = Sz * A[kz];
			const float Bz = Sz * B[kz];
			const float Cz = Sz * C[kz];
			const float T = U * Az + V * Bz + W * Cz;

			#ifdef BACKFACE_CULLING
			if (T < 0.0f || T > hit.t * det) return hit;
			#else
			int det_sign = det > 0.0f ? 1 : -1;
			if (std::xorf(T, det_sign) < 0.0f) ||
			xorf(T,det_sign) > hit.t * xorf(det, det_sign))
			return;
			#endif

			const float rcpDet = 1.0f / det;
			hit.u = U * rcpDet;
			hit.v = V * rcpDet;
			hit.w = W * rcpDet;
			hit.t = T * rcpDet;

			return hit;
		}
	}
}