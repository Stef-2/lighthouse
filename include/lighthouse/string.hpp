#pragma once

#include "vulkan/glm/gtx/string_cast.hpp"

#ifndef VULKAN_HPP_NO_TO_STRING
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_to_string.hpp"
#endif

#include "vkfw.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace lh
{
	namespace string
	{
		using string_t = std::string;

		// concepts that implicitly or explicitly convert to std::string
		template <typename T>
		concept std_convertible = requires(T x) { std::to_string(x); };
		template <typename T>
		concept glm_convertible = requires(T x) { glm::detail::compute_to_string<T>::call(x); };
		template <typename T>
		concept vkfw_convertible = requires(T x) { vkfw::to_string(x); };
		template <typename T>
		concept std_constructible = requires(T x) { string_t(x); } || std::convertible_to<T, string_t>;
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
}
