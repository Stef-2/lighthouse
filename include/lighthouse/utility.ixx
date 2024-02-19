module;

#if INTELLISENSE
#include <concepts>
#include <vector>
#include <ranges>
#endif

export module lighthouse_utility;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	template <typename T>
	using non_owning_ptr = T*;
}