module;

#include "glm/glm.hpp"

module light;

namespace lh
{
	auto light::color() const -> const colors::color&
	{
		return m_color;
	}

	auto light::color(const colors::color& color) -> void
	{
		m_color = glm::vec4 {color.r, color.g, color.b, m_color.a};
	}

	auto light::intensity() const -> const intensity_t&
	{
		return m_color.a;
	}

	auto light::intensity(const intensity_t& intensity) -> void
	{
		m_color.a = intensity;
	}

	auto physical_light::intensity(const intensity_t& intensity) -> void
	{
		light::intensity(intensity);

		// given our current intensity and the inverse square intensity decay law, calculate the distance past which
		// this lights contribution can be discarded
		// threshold is an arbitrary value that we consider to no longer provide a significant contribution to final
		// lighting output
		m_effective_radius = glm::sqrt(m_color.a / physical_light_distance_threshold);
	}

	auto physical_light::effective_radius() const -> const light::intensity_t&
	{
		return m_effective_radius;
	}

	auto physical_light::intensity_at(const entity::position_t& position) const -> light::intensity_t
	{
		// physical light decay is proportional to inverse of the square of the distance between the source and point X
		return m_color.a * (1 / glm::pow(glm::distance(this->m_position, position), 2));
	}

}
