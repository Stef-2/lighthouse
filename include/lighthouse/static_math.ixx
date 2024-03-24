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

import math;

#define EXP_A 184
#define EXP_C 16249

float EXP(float y)
{
	union
	{
		float d;
		struct
		{
#ifdef LITTLE_ENDIAN
			short j, i;
#else
			short i, j;
#endif
		} n;
	} eco;
	eco.n.i = EXP_A * (y) + (EXP_C);
	eco.n.j = 0;
	return eco.d;
}

float LOG(float y)
{
	int* nTemp = (int*)&y;
	y = (*nTemp) >> 16;
	return (y - EXP_C) / EXP_A;
}

float POW(float b, float p)
{
	return EXP(LOG(b) * p);
}

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
			consteval auto abs(T value)
			{
				return value > 0 ? value : -value;
			}

			template <typename T>
				requires std::is_integral_v<T> or std::is_floating_point_v<T>
			consteval auto min(T x, T y)
			{
				return x < y ? x : y;
			}

			template <typename T>
				requires std::is_integral_v<T> or std::is_floating_point_v<T>
			consteval auto max(T x, T y)
			{
				return x > y ? x : y;
			}

			template <typename T>
				requires std::is_integral_v<T> or std::is_floating_point_v<T>
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
			consteval auto pow(T value, std::size_t exponent)
			{
				if (exponent == 0) return static_cast<T>(1);

				auto result = value;

				for (auto i = std::size_t {1}; i < exponent; i++)
					result = result * value;

				return result;
			}

			template <typename T>
			consteval auto mod(T value, T divisor)
			{
				if constexpr (std::is_integral_v<T>) return value % divisor;

				T result = value;

				

				return result / divisor;
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
					
					if (not isvalid(next_term) or not isvalid(result + next_term))
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

					if (not isvalid(next_term) or not isvalid(result + next_term))
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