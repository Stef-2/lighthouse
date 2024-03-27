module;

#if INTELLISENSE
#include "glm/glm.hpp"

#include <cstddef>
#include <numeric>
#include <numbers>
#include <limits>
#include <cmath>
#include <bit>
#include <utility>
#endif

export module static_math;

#if not INTELLISENSE
import std.core;
import glm;
#endif

import math;

export namespace lh
{
	namespace math
	{
		namespace static_math
		{
			constexpr auto iteration_count = 32;

			template <typename T>
				requires std::is_floating_point_v<T>
			consteval auto is_valid(T value)
			{
				const auto is_nan = value != value;
				const auto is_inf = value == std::numeric_limits<T>::infinity();
				const auto is_neg_inf = value == -std::numeric_limits<T>::infinity();
				const auto is_denorm = value <= std::numeric_limits<T>::denorm_min();

				return not is_nan and not is_inf and not is_neg_inf;
			}

			template <typename T>
			consteval auto abs(T value)
			{
				return value > 0 ? value : -value;
			}

			template <typename R = std::int64_t, typename T>
				requires std::is_floating_point_v<T>
			consteval R to_integer(T value)
			{
				if (not is_valid(value)) return 0;

				const auto integer = static_cast<R>(value);
				const auto integer_above = integer + 1;
				constexpr auto epsilon = std::numeric_limits<double>::epsilon() * 4'503'599'628/*'599'627'370'496*/;
				constexpr auto epsilon2 = std::numeric_limits<double>::epsilon();
				//constexpr auto epsilon = std::numeric_limits<T>::epsilon() * 99'999'999'999'999'9;
				const auto floating = static_cast<T>(integer);

				if (static_cast<T>(integer_above) - value < epsilon2) return integer_above else return integer;
			}
			constexpr auto qwe = to_integer(1.9999999999999);
			constexpr auto asd = 7.0 / 3.5;
			template <typename T>
				requires std::is_arithmetic_v<T>
			consteval auto min(T x, T y)
			{
				return x < y ? x : y;
			}

			template <typename T>
				requires std::is_arithmetic_v<T>
			consteval auto max(T x, T y)
			{
				return x > y ? x : y;
			}

			template <typename T>
				requires std::is_arithmetic_v<T>
			consteval auto clamp(T value, T minimum, T maximum)
			{
				return min(max(value, minimum), maximum);
			}
			template <typename T>
				requires std::is_floating_point_v<T>
			consteval auto ceil(T value)
			{
				const auto to_integer = static_cast<std::int64_t>(value);

				if (static_cast<T>(to_integer) == value)
					return to_integer
				else
					return value > 0 ? to_integer + 1 : to_integer;
				
			}

			template <typename T>
				requires std::is_floating_point_v<T>
			consteval auto floor(T value)
			{
				const auto to_integer = static_cast<std::int64_t>(value);

				return to_integer;
			}

			template <typename T>
				requires std::is_floating_point_v<T>
			consteval auto fraction(T value)
			{
				const auto integer = floor(value);

				const auto result = value - static_cast<T>(integer);

				return result;
			}

			template <typename T>
				requires std::is_floating_point_v<T>
			consteval auto round(T value)
			{
				const auto fractional_part = fraction(value);
				const auto default_round = floor(value);

				if (fractional_part >= 0.5)
					return static_cast<std::int64_t>(default_round) + 1;
				else
					return static_cast<std::int64_t>(default_round);
			}
			constexpr auto test_round = round(1.7);
			
			constexpr auto testtt = fraction(1.5);
			constexpr auto omg = 1 / testtt;
			constexpr auto testttt = to_integer(1 / testtt);
			template <typename T, std::size_t N = iteration_count>
			consteval T nth_root(T value, std::size_t n)
			{
				constexpr int K = N;
				T x[K] = {1};
				for (int k = 0; k < K - 1; k++)
				{
					T power = x[k];
					for (size_t i = 1; i < n - 1; i++)
						power = power * x[k];

					x[k + 1] = (1.0 / n) * ((n - 1) * x[k] + value / power/*pow<T, std::size_t>(x[k], n - 1))*/);
				}
				return x[K - 1];
			}

			template <typename T, typename Y>
			consteval auto pow(T value, Y exponent)
			{
				if (value == 0) return static_cast<T>(0);
				if (exponent == 0) return static_cast<T>(1);
				if (exponent == 1) return value;

				if (exponent < 0) return 1 / pow(value, -exponent);
				if (exponent > 0.0 and exponent < 1.0) return static_cast<T>(nth_root<T>(value, 1 / exponent));
				if constexpr (std::is_floating_point_v<Y>)
					if (exponent > 1.0)
					return pow(value, floor(exponent)) * nth_root(value, round(1 / fraction(exponent)));

				auto result = value;

				for (auto i = std::size_t {1}; i < exponent; i++)
					result = result * value;

				return result;
			}
			constexpr auto frac = fraction(2.37);
			constexpr auto one_over = 1 / frac;
			constexpr auto power = pow(4.0, 2.5);
			constexpr auto gggcd = std::gcd(1, 3);
			template <typename T>
			consteval auto mod(T value, T divisor)
			{
				if constexpr (std::is_integral_v<T>) return value % divisor;

				T result = value;

				

				return result / divisor;
			}

			template <typename T>
			consteval auto to_fraction(T value)
			{
				struct asd
				{
					T x, y;
				};

				const auto integer = floor(value);
				const auto frac = fraction(value);

				constexpr auto precision = 1000000000;

				const auto gcd = std::gcd(round(frac * precision), precision);

				const auto numerator = round(frac * precision) / gcd;
				const auto denominator = precision / gcd;

				const std::pair<T, T> asdf = {numerator + integer, denominator};
				const asd xx{numerator, denominator};
				return asdf;
			}

			constexpr auto xxx = to_fraction(3.14);

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
					guess -= (pow(guess, 2) - x) / (2 * guess);

				return guess;
			}
			
			template <typename T, std::size_t N = iteration_count>
			consteval auto sin(T value)
			{
				auto result = value;

				auto sign = std::int8_t {-1};
				auto increment = std::uint32_t {3};

				for (auto i = std::size_t {}; i < N; i++)
				{
					const auto next_term = sign * (pow(value, increment) / factorial(increment));
					
					if (not is_valid(next_term) or not is_valid(result + next_term))
						return result;

					result += next_term;
					increment += 2;
					sign *= -1;
				}

				return result;
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto cos(T value)
			{
				auto result = 1.0;

				auto sign = std::int8_t {-1};
				auto increment = std::uint32_t {2};

				for (auto i = std::size_t {}; i < N; i++)
				{
					const auto next_term = sign * (pow(value, increment) / factorial(increment));

					if (not is_valid(next_term) or not is_valid(result + next_term))
						return result;

					result += next_term;
					increment += 2;
					sign *= -1;
				}

				return result;
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto tan(T value)
			{
				return sin<T, N>(value) / cos<T, N>(value);
			}

			template <typename T, std::size_t N = iteration_count>
			consteval auto ctan(T value)
			{
				return cos<T, N>(value) / sin<T, N>(value);
			}

			constexpr auto t = pow(5.5, 1.704748);

			constexpr auto test_mod = ceil(-126.3);
			constexpr auto test_mod2 = floor(-123.2);

			constexpr auto p = 2.0 + (pow(2.0, 2)-sqrt(2.0));
		}
	}
}