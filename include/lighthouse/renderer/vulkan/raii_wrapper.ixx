module;
#pragma once

export module raii_wrapper;

#if INTELLISENSE
#include "lighthouse/string/string.ixx"
#else
import lighthouse_string;
#endif

export import vulkan;

export namespace lh
{
	namespace vulkan
	{
		template <typename T>
		class raii_wrapper
		{
		public:
			raii_wrapper(T&& object) noexcept : m_object(std::move(object)) {};
			raii_wrapper(std::nullptr_t null = nullptr) : m_object(std::forward<std::nullptr_t>(null)) {};

			// disallow copy constructors, in line with vulkan raii types
			raii_wrapper(const T&) = delete;
			raii_wrapper operator=(const T&) = delete;

			auto operator*() -> T& { return m_object; }
			auto operator*() const -> const T& { return m_object; }

			auto operator->() -> T* { return &m_object; }
			auto operator->() const -> const T* { return &m_object; }

			operator T&() { return m_object; }
			operator const T&() const { return m_object; };

			virtual auto info() const -> lh::string::string_t { return {}; }

		protected:
			T m_object;
		};
	}
}
