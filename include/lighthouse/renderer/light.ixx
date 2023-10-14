module;

#include "glm/vec4.hpp"

export module light;

import entity;
import color;

namespace lh
{
	// abstract base light class, defining common light attributes
	class light
	{
	public:
		using intensity_t = float;

		auto color() const -> const colors::color&;
		auto color(const colors::color&) -> void;
		auto intensity() const -> const intensity_t&;
		virtual auto intensity(const intensity_t&) -> void;

	protected:
		light() = default;

		colors::color m_color;
		intensity_t m_intensity;
	};

	// abstract base physical light class
	// inherits transformation handling mechanisms from entity and common light properties from light
	// acting as a base for lights with physical positions, orientations and an inverse square intensity decay
	class physical_light : public light, public entity
	{
	public:
		using entity::entity;

		auto intensity(const intensity_t&) -> void override final;
		auto effective_radius() const -> const light::intensity_t&;
		auto intensity_at(const entity::position_t&) const -> light::intensity_t;

	protected:
		physical_light() = default;
		light::intensity_t m_effective_radius;
	};

	// specialized physical light, radiates light in all directions
	class point_light : public physical_light
	{
	public:
		struct shader_data
		{
			glm::vec4 m_position;
			glm::vec4 m_color;
			float m_intensity;
		};
	private:
		static constexpr inline auto wtf = sizeof shader_data;
	};

	// specialized physical light, radiates light in a cone
	class spot_light : public physical_light
	{
	public:
		using parameter_precision_t = float;

	private:
		parameter_precision_t m_spread_angle;
		parameter_precision_t m_sharpness;
	};

	// specialized non physical light, radiates parallel light rays with an infinite radius and no decay
	class directional_light : public light
	{
	public:

	private:
		entity::position_t m_position;
		entity::rotation_t m_rotation;
	};

	// specialized non physical light, applies linearly decaying lighting in affected radius
	class ambient_light : public light
	{
	public:

	private:
	};
}

module :private;

namespace lh
{
	// given a physical light's intensity and the inverse square intensity decay law,
	// we can calculate the distance past which a physical light's color contribution can be discarded for added performance
	// threshold is an arbitrary value that we consider to no longer provide a significant contribution to final
	// lighting output
	constexpr auto physical_light_distance_threshold = 100.0f;
}