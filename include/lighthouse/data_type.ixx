module;

#if INTELLISENSE
	#include <cstddef>
	#include <vector>
#endif

export module data_type;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	using float32_t = float;
	using float64_t = double;

	using data_t = std::vector<std::byte>;
}