module;
#pragma once

export module index_format;

#if INTELLISENSE
#include <cstdint>
#else
import std.core;
#endif
export namespace lh
{
	namespace vulkan
	{
		using vertex_index_t = std::uint32_t;
	}
}
