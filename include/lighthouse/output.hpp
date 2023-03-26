#pragma once

#include "lighthouse/static.hpp"
#include "lighthouse/string.hpp"
#include "lighthouse/string_convertible.hpp"

namespace lh
{

	// static utility class that provides custom logging facilities
	class output : static_t
	{
	public:
		friend class engine;

		// string type to be used as the internal buffer
		using string_t = lh::string::string_t;

		// custom buffer
		class buffer
		{
		public:
			auto get_data() const -> std::string_view;
			auto get_last_line() const -> std::string_view;

			static constexpr auto to_string(const string::string_convertible auto& data) -> lh::string::string_t
			{
				if constexpr (string::std_convertible<decltype(data)>)
					return std::to_string(data);

				if constexpr (string::glm_convertible<decltype(data)>)
					return glm::to_string(data);

				if constexpr (string::vkfw_convertible<decltype(data)>)
					return vkfw::to_string(data);

				if constexpr (string::std_constructible<decltype(data)>)
					return lh::string::string_t {data};

				if constexpr (string::vulkan_to_string and string::vulkan_convertible<decltype(data)>)
					return vk::to_string(data);
			}

			// enable std::cout like << operator
			auto operator<<(const string::string_convertible auto& data) -> buffer&
			{
				m_buffer.append(to_string(data).append("\n"));

				if (m_fatal_flag) [[unlikely]]
					output::exit();

				return *this;
			}

			// implicit string conversion
			operator lh::string::string_t() const;

		private:
			lh::string::string_t m_buffer {};
		};

		static auto log() -> buffer&;
		static auto warning() -> buffer&;
		static auto error() -> buffer&;

		static auto fatal() -> buffer&;

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

		return lh::string::string_t {buffer};
	}
}
