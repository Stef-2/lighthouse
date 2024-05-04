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

		memory_mapped_span(const non_owning_ptr<T> pointer, const std::size_t element_count)
			: std::span<T> {pointer, element_count}, m_last_element {this->begin()}
		{}

		auto operator [](std::size_t index)
		{
			if (index > std::distance(m_last_element, this->begin() + index)) m_last_element = this->begin() + index;

			return *this[index];
		}

		auto emplace_back(const T& element)
		{
			*m_last_element = element;
			m_last_element++;
		}

		auto pop_back()
		{ 
			*m_last_element = {};
			m_last_element--;
		}

		auto clear()
		{
			for (auto& element : *this)
				element = {};

			m_last_element = this->begin();
		}

	private:
		std::span<T>::iterator m_last_element;
	};
}