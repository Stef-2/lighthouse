module;

#if INTELLISENSE
#include <cstddef>
#include <vector>
#endif

export module datatype;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	using data_t = std::vector<std::byte>;
}