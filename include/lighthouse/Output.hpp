#pragma once

#include "glm/gtx/string_cast.hpp"
#include "vkfw.hpp"
#include "vulkan.hpp"
#include "vulkan/vulkan_to_string.hpp"

#include "static.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace lh
{
  // concepts that implicitly or explicitly convert to std::string
  namespace string_concept
  {
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

	template <typename T>
	concept string = std_convertible<T> || glm_convertible<T> || vkfw_convertible<T> || std_constructible<T> ||
					 vulkan_convertible<T>;

  }

  // static utility class that provides custom logging facilities
  class output : static_t
  {
  public:
	friend class engine;

	// string type to be used as the internal buffer
	using string = std::string;

	// custom buffer
	class buffer
	{
	public:
	  auto get_data() const -> std::string_view;
	  auto get_last_line() const -> std::string_view;

	  // enable std::cout like << operator
	  auto operator<<(const string_concept::string auto& data) -> buffer&
	  {
		if constexpr (string_concept::std_convertible<decltype(data)>)
		  m_buffer.append(std::to_string(data)).append("\n");

		if constexpr (string_concept::glm_convertible<decltype(data)>)
		  m_buffer.append(glm::to_string(data)).append("\n");

		if constexpr (string_concept::vkfw_convertible<decltype(data)>)
		  m_buffer.append(vkfw::to_string(data)).append("\n");

		if constexpr (string_concept::std_constructible<decltype(data)>)
		  m_buffer.append(data).append("\n");

		if constexpr (string_concept::vulkan_convertible<decltype(data)>)
		  m_buffer.append(vk::to_string(data)).append("\n");

		if (m_fatal_flag) [[unlikely]]
			output::exit();

		return *this;
	  }

	  // implicit string conversion
	  operator lh::output::string() const;

	private:
	  string m_buffer {};
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
}
