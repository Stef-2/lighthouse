module;

#if INTELLISENSE
#include <cstdint>
#endif

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

export module color;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace colors
	{
		class color : glm::vec4
		{
		public:
			using integer_encoding_t = std::uint32_t;

			using glm::vec4::vec4;

			struct hex24
			{const integer_encoding_t color : 24;};

			struct hex32
			{const integer_encoding_t color;};

			color(const glm::vec3&);
			color(const glm::vec4&);
			color(const glm::ivec3&);
			color(const glm::ivec4&);

			color(const hex24&);
			color(const hex32&);

			operator glm::vec4&();
			explicit operator integer_encoding_t() const;

			auto mix(const color&, float ratio = 0.5f) const -> color;

		private:
		};
		/*
		// grayscales
		const color white {1.0f, 1.0f, 1.0f, 1.0f};
		const color black {0.0f, 0.0f, 0.0f, 1.0f};
		const color gray {0.5f, 0.5f, 0.5f, 1.0f};
		const color none {0.0f, 0.0f, 0.0f, 0.0f};

		// primaries
		const color red {1.0f, 0.0f, 0.0f, 1.0f};
		const color green {0.0f, 1.0f, 0.0f, 1.0f};
		const color blue {0.0f, 0.0f, 1.0f, 1.0f};

		// secondaries
		const color yellow {1.0f, 1.0f, 0.0f, 1.0f};
		const color cyan {0.0f, 1.0f, 1.0f, 1.0f};
		const color magenta {1.0f, 0.0f, 1.0f, 1.0f};

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
		const color& violet {blue_magenta};*/
	}
}