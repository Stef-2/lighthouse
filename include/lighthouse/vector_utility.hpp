#pragma once

#include "output.hpp"

#include <vector>
#include <ranges>
#include <ostream>

namespace lh
{
	template<typename T>
	auto find(const std::vector<T>& vector, const T& element)
	{
		return std::ranges::find(vector, element) != vector.end();
	}

	template<lh::string_concept T>
	lh::output::buffer print(const std::vector<T>& vector)
	{
		auto buffer = lh::output::buffer {};

		for (const auto& element : vector)
		{
		  buffer << element;
		}

		return buffer;
	}
}