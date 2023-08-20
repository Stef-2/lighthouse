module;

#if INTELLISENSE
#include <cstdint>
#endif

export module index_format;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		using vertex_index_t = std::uint32_t;
	}
}
