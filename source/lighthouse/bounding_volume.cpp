module;

module bounding_volume;

namespace lh
{
	namespace geometry
	{
		octant::octant() : aabb {}, m_children {} {}

		octant::octant(const aabb& bounding_volume, double size_threshold) : aabb {bounding_volume}, m_children {}
		{
			const auto half_size = size() / 2.0f;

			if (half_size.x <= size_threshold or half_size.y <= size_threshold or half_size.z <= size_threshold) return;

			m_children = std::make_unique<octant[]>(8);
			const auto center = aabb::m_minima + half_size;

			for (std::uint8_t x = 0; x < 2; x++)
				for (std::uint8_t y = 0; y < 2; y++)
					for (std::uint8_t z = 0; z < 2; z++)
					{
						const auto linear_index = x + 2 * (y + 2 * z);
						const auto offset = glm::vec3 {x * half_size.x, y * half_size.y, z * half_size.z};

						m_children[linear_index] = {aabb {{aabb::m_minima + offset}, {center + offset}},
													size_threshold};
					}
		}

		octant::octant(octant&& other) : aabb {other}, m_children {std::exchange(other.m_children, {})} {}

		auto octant::operator=(octant&& other) -> octant&
		{
			aabb::operator=(std::move(other));
			m_children = std::exchange(other.m_children, {});

			return *this;
		}

		octree::octree(const aabb& bounding_volume, const double size_threshold)
			: octant {bounding_volume, size_threshold}, m_depth {}
		{
			auto depth_test = min_dimension();

			while (depth_test > size_threshold)
			{
				m_depth++;
				depth_test /= 2.0f;
			}
		}
	}
}
