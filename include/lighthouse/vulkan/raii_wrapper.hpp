#pragma once

#include "lighthouse/string.hpp"

namespace lh
{
	namespace vulkan
	{

		template <typename T> class vk_wrapper
		{
		public:
			vk_wrapper(T&& object) noexcept : m_object(std::move(object)) {};
			vk_wrapper(nullptr_t null = nullptr) : m_object(null) {};

			// disallow copy constructors, in line with vulkan raii types
			vk_wrapper(const T&) = delete;
			vk_wrapper operator=(const T&) = delete;

			auto operator*() -> T& { return m_object; }
			auto operator*() const -> T& { return m_object; }

			auto operator->() -> T* { return &m_object; }
			auto operator->() const -> T* { return &m_object; }

			operator T&() { return m_object; }

			virtual auto info() const -> lh::string::string_t { return {}; }

		protected:
			mutable T m_object;
		};
	}
}
