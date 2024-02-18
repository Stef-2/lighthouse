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

	template <typename T, typename Y = T*>
	auto vec_ptr_to_value(const auto& vector)
	{
		auto value_vector = std::vector<T> {};
		value_vector.reserve(vector.size());

		for (const auto& element : vector)
			value_vector.emplace_back(**element);

		return value_vector;
	}
}