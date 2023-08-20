module;

#if INTELLISENSE
#include <string>
#endif

export module lighthouse_string;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace string
	{
		using string_t = std::string;
	}
}