module;

#if INTELLISENSE
#include <utility>
#include <cstddef>
#endif

export module raii_wrapper;

#if INTELLISENSE
#else
export import vulkan_hpp;
#endif
import lighthouse_string;

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
