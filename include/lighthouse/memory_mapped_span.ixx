module;

#if INTELLISENSE
#include <cstdint>
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
		memory_mapped_span(const non_owning_ptr<T> pointer, const std::size_t element_count)
			: std::span<T> {pointer, element_count}, m_last_access_element {this->begin()}
		{}

		using std::span<T>::begin;
		using std::span<T>::cbegin;
		using std::span<T>::end;
		using std::span<T>::cend;
		using std::span<T>::data;
		using std::span<T>::size;
		using std::span<T>::empty;

		auto operator[](std::size_t index) -> T&
		{
			if (index > std::distance(m_last_access_element, this->begin() + index))
				m_last_access_element = this->begin() + index;

			return this->data()[index];
		}

		auto emplace_back(const T& element) -> void
		{
			*m_last_access_element = element;
			m_last_access_element++;
		}

		auto pop_back() -> void
		{
			*m_last_access_element = {};
			m_last_access_element--;
		}

		auto clear() -> void
		{
			for (auto& element : *this)
				element = {};

			m_last_access_element = this->begin();
		}

		auto address() const -> const std::uintptr_t
		{
			//const auto to_void = static_cast<void*>(data());

			return reinterpret_cast<std::uintptr_t>(data());
		}

	protected:
		std::span<T>::iterator m_last_access_element;
	};
}