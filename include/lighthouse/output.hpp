#pragma once

#include "lighthouse/static.hpp"
#include "lighthouse/string/string.hpp"
#include "lighthouse/string/string_convertible.hpp"
#include "lighthouse/file_type.hpp"

#include <filesystem>
#include <fstream>

namespace lh
{

	// static utility class that provides custom logging facilities
	class output : static_t
	{
	public:
		friend class engine;

		// custom buffer
		class buffer
		{
		public:
			auto data() const -> std::string_view;
			auto last_line() const -> std::string_view;

			static constexpr auto to_string(const string::string_convertible auto& data) -> lh::string::string_t
			{
				using type = decltype(data);

				if constexpr (string::std_convertible<type>)
					return std::to_string(data);

				if constexpr (string::glm_convertible<type>)
					return glm::to_string(data);

				if constexpr (string::vkfw_convertible<type>)
					return vkfw::to_string(data);

				if constexpr (string::std_constructible<type>)
					return lh::string::string_t {data};

				if constexpr (string::vulkan_to_string and string::vulkan_convertible<type>)
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

		template <typename T>
		static auto write_file(const std::filesystem::path& path,
							   const std::span<T>& data,
							   const std::iostream::openmode& open_mode = std::iostream::out | std::iostream::trunc)
			-> void
		{
			auto file_stream = std::ofstream {path, open_mode};

			file_stream.write(reinterpret_cast<char*>(data.data()), data.size());
			file_stream.close();
		}

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
	template <lh::string::string_convertible_input_range T>
	auto to_string(const T& range)
	{
		auto buffer = lh::output::buffer {};

		for (const auto& element : range)
			buffer << element;

		return lh::string::string_t {buffer};
	}
}
