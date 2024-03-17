module;

#if INTELLISENSE
#include "glm/glm.hpp"

#include <cstddef>
#include <numeric>
#include <numbers>
#include <limits>
#include <cmath>
#endif

export module static_math;

#if not INTELLISENSE
import std.core;
import glm;
#endif
export namespace lh
{
	namespace math
	{
		namespace static_math
		{
			constexpr auto iteration_count = 32;

			template <typename T>
				requires std::is_floating_point_v<T>
			consteval auto isvalid(T value)
			{
				const auto is_nan = value != value;
				const auto is_inf = value == std::numeric_limits<T>::infinity();
				const auto is_neg_inf = value == -std::numeric_limits<T>::infinity();
				const auto is_denorm = value <= std::numeric_limits<T>::denorm_min();

				return not is_nan and not is_inf and not is_neg_inf;
			}

			template <typename T>
			consteval auto pow(T value, std::size_t exponent)
			{
				auto result = value;

				for (auto i = std::size_t {1}; i < exponent; i++)
					result = result * value;

				return result;
			}

			template <typename T>
				requires std::is_integral_v<T>
			consteval auto factorial(T value)
			{
				auto result = std::size_t {1};

				for (auto i = std::size_t {1}; i <= value; i++)
					result *= i;

				return result;
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto sqrt(T x)
			{
				T guess = 1.0;
				
				for (auto i = std::size_t {1}; i <= N; i++)
					guess -= (guess * guess - x) / (2 * guess);

				return guess;
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto sin(T value)
			{
				T temprorary = value;
				T sine = temprorary;

				for (auto i = std::size_t {1}; i < N; ++i)
				{
					const T mult = -value * value / ((2 * i + 1) * (2 * i));
					temprorary *= mult;
					sine += temprorary;
				}
				return sine;
			}
			
			template <typename T, std::size_t N = iteration_count>
			consteval auto test_sine(T value)
			{
				auto result = value;

				auto sign = std::int8_t {-1};
				auto increment = std::uint32_t {3};

				for (auto i = std::size_t {}; i < N; i++)
				{
					const auto next_term = sign * (pow(value, increment) / factorial(increment));
					
					if (not isvalid(next_term) or not isvalid(result + next_term))
						return result;

					result += next_term;
					increment += 2;
					sign *= -1;
				}

				return result;
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto test_cose(T value)
			{
				auto result = 1.0;

				auto sign = std::int8_t {-1};
				auto increment = std::uint32_t {2};

				for (auto i = std::size_t {}; i < N; i++)
				{
					const auto next_term = sign * (pow(value, increment) / factorial(increment));

					if (not isvalid(next_term) or not isvalid(result + next_term)) return result;

					result += next_term;
					increment += 2;
					sign *= -1;
				}

				return result;
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto cos(T x)
			{
				const auto div = static_cast<std::size_t>(x / std::numbers::pi_v<T>);
				x = x - (div * std::numbers::pi_v<T>);

				const auto sign = div % 2 != 0 ? std::int8_t {-1} : std::int8_t {1};

				T result = 1.0;
				T inter = 1.0;

				for (auto i = std::size_t {1}; i <= N; i++)
				{
					const T comp = 2.0 * i;
					const T den = comp * (comp - 1.0);
					inter *= x * x / den;

					result += i % 2 == 0 ? inter : -inter;
				}

				return sign * result;
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto tan(T value)
			{
				return sin<T, N>(value) / cos<T, N>(value);
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto cotan(T value)
			{
				return cos<T, N>(value) / sin<T, N>(value);
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto atan(T value)
			{
				assert(value > 0.0 and value <= 1.0);

				auto result = value;

				auto sign = std::int8_t {-1};
				auto increment = std::uint32_t {3};

				for (auto i = std::size_t {}; i < N; i++)
				{
					const auto next_term = sign * (pow(value, increment) / increment);

					if (not isvalid(next_term) or not isvalid(result + next_term)) return result;

					result += next_term;
					increment += 2;
					sign *= -1;
				}

				return result;
			}

			constexpr double operator""_to_radians(long double degrees) { return degrees * std::numbers::pi_v<decltype(degrees)> / 180; }
			constexpr double operator""_to_degrees(long double radians) { return radians * 180 / std::numbers::pi_v<decltype(radians)>; }

			constexpr auto test_sin = sin<double, 16>(90.0_to_radians);
			constexpr auto test_cos = cos<double, 32>(90.0_to_radians);
			constexpr auto test_tan = tan<double, 32>(60.0_to_radians);
			constexpr auto test_cot = cotan<double, 32>(60.0_to_radians);
			constexpr auto test_sqrt = sqrt(3.0);
			constexpr auto test_fact = factorial(5);
			constexpr auto test_pow = pow(2, 3);

			constexpr auto radians = 30.0_to_radians;
			constexpr auto test_sine2 = test_sine<double, 32>(90.0_to_radians);
			constexpr auto test_cos2 = test_cose<double, 32>(90.0_to_radians);

			constexpr auto test_valid = isvalid(34.0f);

			constexpr auto test_tan2 = test_sine(60.0_to_radians) / test_cose(60.0_to_radians);
			constexpr auto test_atan = atan(3.0);
			constexpr auto qpi = std::numbers::pi / 4;
		}
	}
}