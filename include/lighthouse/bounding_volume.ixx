module;

#if INTELLISENSE
#include "glm/vec3.hpp"
#endif

export module bounding_volume;

import geometry;

#if not INTELLISENSE
import glm;
#endif

import std;

export namespace lh
{
	namespace geometry
	{
		class octree
		{
		public:
			enum class octant_position
			{
				near_bottom_left,
				near_bottom_right,
				near_top_left,
				near_top_right,
				far_bottom_left,
				far_bottom_right,
				far_top_left,
				far_top_right
			};

			struct octant
			{
				octant() {}
				octant(const aabb& bounding_volume, double size_threshold) : m_aabb {bounding_volume}
				{
					const auto half_size = (m_aabb.m_maxima - m_aabb.m_minima) / 2.0f;

					if (half_size.x <= size_threshold or half_size.y <= size_threshold or half_size.z <= size_threshold)
						return;

					m_children = std::make_unique_for_overwrite<octant[]>(8);
					const auto center = m_aabb.m_minima + half_size;

					for (std::uint8_t x = 0; x < 2; x++)
						for (std::uint8_t y = 0; y < 2; y++)
							for (std::uint8_t z = 0; z < 2; z++)
							{
								const auto linear_index = x + 2 * (y + 2 * z);
								const auto offset = glm::vec3 {x * half_size.x, y * half_size.y, z * half_size.z};

								m_children[linear_index] = {aabb {{m_aabb.m_minima + offset}, {center + offset}},
															size_threshold};
							}
				}

				octant(const octant&) = delete;
				auto operator=(const octant&) -> octant& = delete;
				octant(octant&& rhs) : m_aabb {rhs.m_aabb}, m_children {std::exchange(rhs.m_children, {})} {}
				auto operator=(octant&& rhs) -> octant&
				{
					m_aabb = rhs.m_aabb;
					m_children = std::exchange(rhs.m_children, {});

					return *this;
				}

				aabb m_aabb;
				std::unique_ptr<octant[]> m_children;
			};

			octree(const glm::vec3& center, const glm::vec3& extents, const double size_threshold);
			octree(const aabb& bounding_volume, const double size_threshold)
				: m_bounding_volume {bounding_volume, size_threshold}
			{}

			// private:
			octant m_bounding_volume;
		};
	}
}
