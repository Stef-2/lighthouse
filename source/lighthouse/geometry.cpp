module;

module geometry;

namespace lh
{
	namespace geometry
	{
		rotation_t::rotation_t() : quaternion_t {} {}

		rotation_t::rotation_t(scalar_t x, scalar_t y, scalar_t z) : quaternion_t {{x, y, z}} {}

		rotation_t::rotation_t(const quaternion_t& value) : quaternion_t {value} {}

		rotation_t::rotation_t(const normal_t& value) : quaternion_t {value} {}

		auto rotation_t::operator=(const quaternion_t& value) -> void
		{
			static_cast<quaternion_t>(*this) = value;
		}

		auto rotation_t::operator=(const normal_t& value) -> void
		{
			static_cast<quaternion_t>(*this) = quaternion_t {value};
		}

		auto rotation_t::rotate(const quaternion_t& value) -> void
		{
			*this *= value;
		}

		auto rotation_t::rotate(const normal_t& value) -> void
		{
			*this *= quaternion_t {value};
		}

		auto rotation_t::euler_degrees_cast() const -> const normal_t
		{
			return glm::degrees(glm::eulerAngles(*this));
		}

		auto rotation_t::euler_radians_cast() const -> const normal_t
		{
			return glm::eulerAngles(*this);
		}

		auto rotation_t::matrix_cast() const -> const transformation_t
		{
			return glm::mat4_cast(*this);
		}

		auto rotation_t::dot_product(const rotation_t& other) const -> const scalar_t
		{
			return glm::dot(static_cast<quaternion_t>(*this), static_cast<quaternion_t>(other));
		}

		auto rotation_t::cross_product(const rotation_t& other) const -> const quaternion_t
		{
			return glm::cross(*this, other);
		}

		rotation_t::operator quaternion_t&()
		{
			return *this;
		}

		rotation_t::operator const quaternion_t&() const
		{
			return *this;
		}

		rotation_t::operator normal_t()
		{
			return euler_radians_cast();
		}

		rotation_t::operator const normal_t()
		{
			return euler_radians_cast();
		}

		rotation_t::operator transformation_t()
		{
			return matrix_cast();
		}

		rotation_t::operator const transformation_t() const
		{
			return matrix_cast();
		}

		auto aabb::center() const -> const position_t
		{
			return m_minima + (m_maxima - m_minima) / 2.0f;
		}

		auto aabb::size() const -> const vec3_t
		{
			return {m_maxima - m_minima};
		}

		auto aabb::min_dimension() const -> const scalar_t
		{
			const auto size = aabb::size();

			return std::min({size.x, size.y, size.z});
		}

		auto aabb::max_dimension() const -> const scalar_t
		{
			const auto size = aabb::size();

			return std::max({size.x, size.y, size.z});
		}

		auto aabb::transformation() const -> const transformation_t
		{
			const auto scale = size();
			const auto center = m_minima + scale / 2.0f;

			return {scale.x,
					0.0f,
					0.0f,
					center.x,

					0.0f,
					scale.y,
					0.0f,
					center.y,

					0.0f,
					0.0f,
					scale.z,
					center.z,

					0.0f,
					0.0f,
					0.0f,
					1.0f};
		}
	}
}
