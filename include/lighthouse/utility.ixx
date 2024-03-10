module;

#if INTELLISENSE
#include <concepts>
#include <vector>
#include <ranges>
#include <concepts>
#endif

export module lighthouse_utility;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace concepts
	{
		template <typename T, typename... Ts>
		concept is_any = std::disjunction_v<std::is_same<T, Ts>...>;

		template <typename T, typename... Ts>
		concept are_same = std::conjunction_v<std::is_same<T, Ts>...>;
	}

	template <typename T>
	using non_owning_ptr = T*;

	struct empty {};
}