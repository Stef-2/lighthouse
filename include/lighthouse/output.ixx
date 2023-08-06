module;
#pragma once

#include "glm/ext.hpp"

export module output;

#if INTELLISENSE
#include "lighthouse/file_type.ixx"
#include "lighthouse/string/string.ixx"
#include "lighthouse/string/string_convertible.ixx"
#else
import string_convertible;
import lighthouse_string;
import file_type;
import std.core;
import std.filesystem;
#endif

namespace lh
{
	// utility namespace that provides custom logging facilities
	namespace output
	{
		export auto exit() -> void;
		auto n_fatal_flag = false;

		// custom text buffer
		export class buffer
		{
		public:
			auto data() const -> std::string_view;
			auto last_line() const -> std::string_view;

			constexpr auto to_string(const string::string_convertible auto& data) -> lh::string::string_t
			{
				using type_t = decltype(data);

				if constexpr (string::std_convertible<type_t>)
					return std::to_string(data);

				if constexpr (string::glm_convertible<type_t>)
					return glm::to_string(data);

				if constexpr (string::vkfw_convertible<type_t>)
					return vkfw::to_string(data);

				if constexpr (string::std_constructible<type_t>)
					return lh::string::string_t {data};

				if constexpr (string::vulkan_to_string and string::vulkan_convertible<type_t>)
					return vk::to_string(data);
			}

			// enable std::cout like << operator
			auto operator<<(const string::string_convertible auto& data) -> buffer&
			{
				m_buffer.append(to_string(data).append("\n"));
				
				if (n_fatal_flag) [[unlikely]]
					lh::output::exit();
		
				return *this;
			}

			// implicit string conversion
			operator string::string_t() const;

		private:
			string::string_t m_buffer {};
		};
		
		export template <typename T>
		auto write_file(const std::filesystem::path& path,
						const std::span<T>& data,
						const std::iostream::openmode& open_mode = std::iostream::out | std::iostream::trunc) -> void
		{
			auto file_stream = std::ofstream {path, open_mode};

			file_stream.write(reinterpret_cast<char*>(data.data()), data.size());
			file_stream.close();
		}
		
		export auto log() -> buffer&;
		export auto warning() -> buffer&;
		export auto error() -> buffer&;

		export auto fatal() -> buffer&;

		export auto initialize() -> void;
		export auto dump_logs(std::ostream&) -> void;

		
		auto n_log = buffer {};
		auto n_warning = buffer {};
		auto n_error = buffer {};


	};
}

export
{
	// enable output into std::ostream
	auto operator<<(std::ostream& stream, lh::output::buffer& buffer)->std::ostream&;

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
