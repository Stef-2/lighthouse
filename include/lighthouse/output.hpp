#pragma once

#include "glm/gtx/string_cast.hpp"
#include "vkfw.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_to_string.hpp"

#include "static.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace lh
{
	namespace string
	{
		// concepts that implicitly or explicitly convert to std::string
		template <typename T>
		concept std_convertible = requires(T x) { std::to_string(x); };
		template <typename T>
		concept glm_convertible = requires(T x) { glm::detail::compute_to_string<T>::call(x); };
		template <typename T>
		concept vkfw_convertible = requires(T x) { vkfw::to_string(x); };
		template <typename T>
		concept std_constructible = requires(T x) { std::string(x); } || std::convertible_to<T, std::string>;
		template <typename T>
		concept vulkan_convertible = requires(T x) { vk::to_string(x); };

		// combined string convertible concepts
		template <typename T>
		concept string_convertible = std_convertible<T> || glm_convertible<T> || vkfw_convertible<T> ||
									 std_constructible<T> || vulkan_convertible<T>;

		// concept of a container range holding string convertible types
		template <typename T>
		concept string_convertible_input_range = std::ranges::input_range<T> &&
												 lh::string::string_convertible<std::ranges::range_value_t<T>>;
	}

	// static utility class that provides custom logging facilities
	class output : static_t
	{
	public:
		friend class engine;

		// string type to be used as the internal buffer
		using string_t = std::string;

		// custom buffer
		class buffer
		{
		public:
			auto get_data() const -> std::string_view;
			auto get_last_line() const -> std::string_view;

			// enable std::cout like << operator
			auto operator<<(const string::string_convertible auto& data) -> buffer&
			{
				m_buffer.append(output::to_string(data).append("\n"));

				if (m_fatal_flag) [[unlikely]]
					output::exit();

				return *this;
			}

			// implicit string conversion
			operator lh::output::string_t() const;

		private:
			string_t m_buffer;
		};

		static auto log() -> buffer&;
		static auto warning() -> buffer&;
		static auto error() -> buffer&;

		static auto fatal() -> buffer&;

		static constexpr auto to_string(const string::string_convertible auto& data) -> lh::output::string_t
		{
			if constexpr (string::std_convertible<decltype(data)>)
				return std::to_string(data);

			if constexpr (string::glm_convertible<decltype(data)>)
				return glm::to_string(data);

			if constexpr (string::vkfw_convertible<decltype(data)>)
				return vkfw::to_string(data);

			if constexpr (string::std_constructible<decltype(data)>)
				return lh::output::string_t {data};

			if constexpr (string::vulkan_convertible<decltype(data)>)
				return vk::to_string(data);
		}

	private:
		static auto initialize() -> void;
		static auto dump_logs(std::ostream&) -> void;
		static auto exit() -> void;

		static inline auto m_log = buffer {};
		static inline auto m_warning = buffer {};
		static inline auto m_error = buffer {};

		static inline auto m_fatal_flag = false;
	};

	// enable output into std::ostream
	auto operator<<(std::ostream& stream, lh::output::buffer& buffer) -> std::ostream&;

	// utility function that allows printing of any container holding string convertible types
	template <lh::string::string_convertible_input_range T> auto to_string(const T& range)
	{
		auto buffer = lh::output::buffer {};

		for (const auto& element : range)
			buffer << element;

		return lh::output::string_t {buffer};
	}
}
