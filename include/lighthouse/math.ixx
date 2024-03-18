module;

#if INTELLISENSE
#include <numbers>
#include <concepts>
#endif

export module math;

import data_type;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	constexpr float64_t operator""_to_radians(long double degrees) { return degrees * std::numbers::pi_v<decltype(degrees)> / 180; }
	constexpr float64_t operator""_to_degrees(long double radians) { return radians * 180 / std::numbers::pi_v<decltype(radians)>; }

	constexpr float64_t operator""_deg(long double degrees) { return degrees * std::numbers::pi_v<decltype(degrees)> / 180; }


	namespace math
	{
		template <typename T>
			requires std::is_floating_point_v<T>
		constexpr auto to_radians(T degrees) { return degrees * std::numbers::pi_v<decltype(degrees)> / 180; }

		template <typename T>
			requires std::is_floating_point_v<T>
		constexpr auto to_degrees(T radians) { return radians * 180 / std::numbers::pi_v<decltype(radians)>; }
	}
}