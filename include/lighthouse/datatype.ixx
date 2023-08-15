module;
#pragma once

export module datatype;

#if INTELLISENSE
#include <vector>
#else
import std.core;
#endif

export namespace lh
{
	using data_t = std::vector<std::byte>;
}