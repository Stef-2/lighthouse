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
		struct octant : public aabb
		{
			octant();
			octant(const aabb& bounding_volume, double size_threshold);

			octant(const octant&) = delete;
			auto operator=(const octant&) -> octant& = delete;
			octant(octant&& other);
			auto operator=(octant&& other) -> octant&;

			std::unique_ptr<octant[]> m_children;
		};

		class octree : public octant
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

			octree(const glm::vec3& center, const glm::vec3& extents, const double size_threshold);
			octree(const aabb& bounding_volume, const double size_threshold);

			std::size_t m_depth;
		};
	}
}
