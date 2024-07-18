module;

#if INTELLISENSE
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#endif

export module color;

#if not INTELLISENSE
import glm;
#endif

import std;

export namespace lh
{
	namespace colors
	{
		enum class color_mode
		{
			rgb,
			hsv,
			hsl = hsv,
			default_mode = rgb
		};

		enum class color_component
		{
			red,
			green,
			blue,
			alpha,
			hue,
			saturation,
			value,
			luminosity
		};

		class color : public glm::vec4
		{
		public:
			using color_component_t = decltype(glm::vec4::a);
			using integer_encoding_t = std::uint32_t;

			struct hex_24
			{const integer_encoding_t color;};

			struct hex_32
			{const integer_encoding_t color;};
			
			color();
			color(const color_component_t&,
				  const color_component_t&,
				  const color_component_t&,
				  const color_component_t& = 1.0f,
				  color_mode = color_mode::default_mode);

			color(const glm::vec3&, color_mode = color_mode::default_mode);
			color(const glm::vec4&, color_mode = color_mode::default_mode);
			color(const glm::ivec3&, color_mode = color_mode::default_mode);
			color(const glm::ivec4&, color_mode = color_mode::default_mode);

			color(const hex_24&, color_mode = color_mode::default_mode);
			color(const hex_32&, color_mode = color_mode::default_mode);

			auto color_mode() const -> const colors::color_mode&;
			auto color_mode(const colors::color_mode&) -> void;
			auto mix(const color&, float ratio = 0.5f) const -> color;

			auto component(color_component) const -> color_component_t;

			operator glm::vec3() const;
			operator glm::vec4&();
			explicit operator integer_encoding_t() const;

		private:
			colors::color_mode m_color_mode;
		};
		
		// grayscales
		const color white {glm::vec4 {1.0f, 1.0f, 1.0f, 1.0f}};
		const color black {glm::vec4 {0.0f, 0.0f, 0.0f, 1.0f}};
		const color gray {glm::vec4 {0.5f, 0.5f, 0.5f, 1.0f}};
		const color none {glm::vec4 {0.0f, 0.0f, 0.0f, 0.0f}};
		
		// primaries
		const color red {glm::vec4 {1.0f, 0.0f, 0.0f, 1.0f}};
		const color green {glm::vec4 {0.0f, 1.0f, 0.0f, 1.0f}};
		const color blue {glm::vec4 {0.0f, 0.0f, 1.0f, 1.0f}};

		// secondaries
		const color yellow {glm::vec4 {1.0f, 1.0f, 0.0f, 1.0f}};
		const color cyan {glm::vec4 {0.0f, 1.0f, 1.0f, 1.0f}};
		const color magenta {glm::vec4 {1.0f, 0.0f, 1.0f, 1.0f}};

		// tertiaries
		const color red_yellow {red.mix(yellow)};
		const color green_yellow {green.mix(yellow)};
		const color green_cyan {green.mix(cyan)};
		const color blue_cyan {blue.mix(cyan)};
		const color red_magenta {red.mix(magenta)};
		const color blue_magenta {blue.mix(magenta)};

		// regular / common names for tertiaries
		const color& orange {red_yellow};
		const color& chartreuse {green_yellow};
		const color& spring_green {green_cyan};
		const color& azure {blue_cyan};
		const color& rose {red_magenta};
		const color& violet {blue_magenta};
	}
}