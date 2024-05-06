module;

#if INTELLISENSE
#include <span>
#endif

export module memory_mapped_span;

import lighthouse_utility;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	template <typename T>
	class memory_mapped_span : private std::span<T>
	{
	public:
		using std::span<T>::begin;
		using std::span<T>::cbegin;
		using std::span<T>::end;
		using std::span<T>::cend;
		using std::span<T>::data;
		using std::span<T>::size;
		using std::span<T>::empty;

		memory_mapped_span(const non_owning_ptr<T>, const std::size_t);

		auto operator[](std::size_t) -> T&;
		auto emplace_back(const T&) -> void;
		auto pop_back() -> void;
		auto clear() -> void;

	protected:
		std::span<T>::iterator m_last_access_element;
	};
}