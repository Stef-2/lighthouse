module;

#include "glm/glm.hpp"
#include "glm/gtx/color_space.hpp"

module color;

namespace lh
{
	namespace colors
	{
		constexpr inline auto integer_normalization_factor = 0xFF;

		color::color() : glm::vec4 {}, m_color_mode {color_mode::rgb} {}

		color::color(const color_component_t& x,
					 const color_component_t& y,
					 const color_component_t& z,
					 const color_component_t& w,
					 colors::color_mode color_mode)
			: glm::vec4 {x, y, z, w}, m_color_mode {color_mode}
		{}

		color::color(const glm::vec3& vec3, colors::color_mode color_mode)
			: glm::vec4 {vec3.r, vec3.g, vec3.b, 1.0f}, m_color_mode {color_mode}
		{}
		color::color(const glm::vec4& vec4, colors::color_mode color_mode) : glm::vec4 {vec4}, m_color_mode {color_mode}
		{}
		color::color(const glm::ivec3& ivec3, colors::color_mode color_mode)
			: glm::vec4 {ivec3.r / integer_normalization_factor,
						 ivec3.g / integer_normalization_factor,
						 ivec3.b / integer_normalization_factor,
						 1.0f},
			  m_color_mode {color_mode}
		{}

		color::color(const glm::ivec4& ivec4, colors::color_mode color_mode)
			: glm::vec4 {ivec4.r / integer_normalization_factor,
						 ivec4.g / integer_normalization_factor,
						 ivec4.b / integer_normalization_factor,
						 ivec4.a / integer_normalization_factor},
			  m_color_mode {color_mode}
		{}

		color::color(const hex_24& hex_24, colors::color_mode color_mode)
			: glm::vec4 {static_cast<float>(((hex_24.color >> 24) & integer_normalization_factor) /
											integer_normalization_factor),
						 static_cast<float>(((hex_24.color >> 16) & integer_normalization_factor) /
											integer_normalization_factor),
						 static_cast<float>(((hex_24.color >> 8) & integer_normalization_factor) /
											integer_normalization_factor),
						 1.0f},
			  m_color_mode {color_mode}
		{}

		color::color(const hex_32& hex_32, colors::color_mode color_mode)
			: glm::vec4 {static_cast<float>(((hex_32.color >> 24) & integer_normalization_factor) /
											integer_normalization_factor),
						 static_cast<float>(((hex_32.color >> 16) & integer_normalization_factor) /
											integer_normalization_factor),
						 static_cast<float>(((hex_32.color >> 8) & integer_normalization_factor) /
											integer_normalization_factor),
						 static_cast<float>(((hex_32.color >> 0) & integer_normalization_factor) /
											integer_normalization_factor)},
			  m_color_mode {color_mode}
		{}

		auto color::color_mode() const -> const colors::color_mode&
		{
			return m_color_mode;
		}

		auto color::color_mode(const colors::color_mode& color_mode) -> void
		{
			if (color_mode == m_color_mode)
				return;

			const auto self = glm::vec3 {*this};

			if (color_mode == color_mode::hsl)
				*this = {glm::rgbColor(self), color_mode};
			else
				*this = {glm::hsvColor(self), color_mode};
		}

		color::operator glm::vec3() const
		{
			return {this->r, this->g, this->b};
		}

		color::operator glm::vec4&()
		{
			return static_cast<glm::vec4&>(*this);
		}

		color::operator integer_encoding_t() const
		{
			return ((static_cast<int>(this->r) * integer_normalization_factor) << 24) +
				   ((static_cast<int>(this->g) * integer_normalization_factor) << 16) +
				   ((static_cast<int>(this->b) * integer_normalization_factor) << 8) +
				   ((static_cast<int>(this->a) * integer_normalization_factor) << 0);
		}

		auto color::mix(const color& other, float ratio) const -> color
		{
			return color {glm::mix(static_cast<const glm::vec4&>(*this), static_cast<const glm::vec4&>(other), ratio)};
		}

		auto color::component(color_component component) const -> color_component_t
		{
			// alpha is the same for all color modes
			if (std::to_underlying(component) == std::to_underlying(color_component::alpha))
				return (*this)[std::to_underlying(component)];

			// luminosity is computed only for rgb color mode
			if (std::to_underlying(component) == std::to_underlying(color_component::luminosity))
				if (m_color_mode == color_mode::rgb)
					return glm::luminosity(static_cast<glm::vec3>(*this));
				else
				{
					auto converted = glm::rgbColor(static_cast<glm::vec3>(*this));
					return glm::luminosity(static_cast<glm::vec3>(converted));
				}

			// requesting color components this color doesn't have already encoded requres conversion
			const auto requesting_rgb_components = std::to_underlying(component) <=
												   std::to_underlying(color_component::blue);

			if (requesting_rgb_components and m_color_mode == color_mode::rgb)
				return (*this)[std::to_underlying(component)];
			else if (requesting_rgb_components and m_color_mode != color_mode::rgb)
			{
				auto converted = glm::rgbColor(static_cast<glm::vec3>(*this));
				return converted[std::to_underlying(component)];
			}

			if (not requesting_rgb_components and m_color_mode == color_mode::hsv)
				return (*this)[std::to_underlying(component)];
			else if (not requesting_rgb_components and m_color_mode != color_mode::hsv)
			{
				auto converted = glm::hsvColor(static_cast<glm::vec3>(*this));
				return converted[std::to_underlying(component)];
			}
		}
	}
}
