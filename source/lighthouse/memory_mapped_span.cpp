module;

module memory_mapped_span;

namespace lh
{
	template <typename T>
	memory_mapped_span<T>::memory_mapped_span(const non_owning_ptr<T> pointer, const std::size_t element_count)
		: std::span<T> {pointer, element_count}, m_last_access_element {this->begin()}
	{}

	template <typename T>
	auto memory_mapped_span<T>::operator[](std::size_t index) -> T&
	{
		if (index > std::distance(m_last_access_element, this->begin() + index))
			m_last_access_element = this->begin() + index;

		return *this[index];
	}

	template <typename T>
	auto memory_mapped_span<T>::emplace_back(const T& element) -> void
	{
		*m_last_access_element = element;
		m_last_access_element++;
	}

	template <typename T>
	auto memory_mapped_span<T>::pop_back() -> void
	{
		*m_last_access_element = {};
		m_last_access_element--;
	}

	template <typename T>
	auto memory_mapped_span<T>::clear() -> void
	{
		for (auto& element : *this)
			element = {};

		m_last_access_element = this->begin();
	}
}
