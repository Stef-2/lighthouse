module;

#include "glm/glm.hpp"

module color;

namespace lh
{
	namespace colors
	{
		// constexpr inline auto integer_normalization_factor = 0xFF;

		color::color(const glm::vec3& vec3) : vec {vec3, 1.0f} {}
		color::color(const glm::vec4& vec4) : vec {vec4} {}
		/*
		color::color(const glm::ivec3& ivec3) : glm::vec4 {ivec3 / integer_normalization_factor, 1.0f} {}

		color::color(const glm::ivec4& ivec4) : glm::vec4 {ivec4 / integer_normalization_factor} {}*/
		/*
		color::color(const hex24& hex24)
			: glm::vec4 {((hex24.color >> 24) & integer_normalization_factor) / integer_normalization_factor,
						 ((hex24.color >> 16) & integer_normalization_factor) / integer_normalization_factor,
						 ((hex24.color >> 8) & integer_normalization_factor) / integer_normalization_factor,
						 1.0f}
		{}

		color::color(const hex32& hex32)
			: glm::vec4 {((hex32.color >> 24) & integer_normalization_factor) / integer_normalization_factor,
						 ((hex32.color >> 16) & integer_normalization_factor) / integer_normalization_factor,
						 ((hex32.color >> 8) & integer_normalization_factor) / integer_normalization_factor,
						 ((hex32.color >> 0) & integer_normalization_factor) / integer_normalization_factor}
		{}
		*//*
		color::operator glm::vec4&()
		{
			return static_cast<glm::vec4&>(*this);
		}

		color::operator integer_encoding_t() const {}

		auto color::mix(const color& other, float ratio) const -> color
		{
			return color {glm::mix(static_cast<const glm::vec4&>(*this), static_cast<const glm::vec4&>(other), ratio)};
		}*/
	}
}
