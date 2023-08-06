module;

#if INTELLISENSE
#include <vector>
#else
import std.core;
#endif

export module datatype;

export namespace lh
{
	using data_t = std::vector<std::byte>;
}