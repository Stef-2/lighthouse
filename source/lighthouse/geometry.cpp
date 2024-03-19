module;

module geometry;

namespace lh
{
	namespace geometry
	{
		direction_t::direction_t() : quaternion_t {} {}

		direction_t::direction_t(scalar_t x, scalar_t y, scalar_t z) : quaternion_t {{x, y, z}} {}

		direction_t::direction_t(const quaternion_t& value) : quaternion_t {value} {}

		direction_t::direction_t(const normal_t& value) : quaternion_t {value} {}

		auto direction_t::operator=(const quaternion_t& value) -> void { static_cast<quaternion_t>(*this) = value; }

		auto direction_t::operator=(const normal_t& value) -> void { static_cast<quaternion_t>(*this) = quaternion_t {value}; }

		auto direction_t::rotate(const quaternion_t& value) -> void
		{
			*this *= value;
		}

		auto direction_t::rotate(const normal_t& value) -> void
		{
			*this *= quaternion_t {value};
		}

		auto direction_t::euler_degrees_cast() const -> const normal_t
		{
			return glm::degrees(glm::eulerAngles(*this));
		}

		auto direction_t::euler_radians_cast() const -> const normal_t
		{
			return glm::eulerAngles(*this);
		}

		auto direction_t::matrix_cast() const -> const transformation_t
		{
			return glm::mat4_cast(*this);
		}

		auto direction_t::dot_product(const direction_t& other) const -> const scalar_t
		{
			return glm::dot(static_cast<quaternion_t>(*this), static_cast<quaternion_t>(other));
		}

		auto direction_t::cross_product(const direction_t& other) const -> const quaternion_t
		{
			return glm::cross(*this, other);
		}

		direction_t::operator quaternion_t&()
		{
			return *this;
		}

		direction_t::operator const quaternion_t&() const
		{
			return *this;
		}

		direction_t::operator normal_t()
		{
			return euler_radians_cast();
		}

		direction_t::operator const normal_t()
		{
			return euler_radians_cast();
		}

		direction_t::operator transformation_t()
		{
			return matrix_cast();
		}

		direction_t::operator const transformation_t() const
		{
			return matrix_cast();
		}
	}
}
