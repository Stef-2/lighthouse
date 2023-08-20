module;

#if INTELLISENSE
#include <vector>
#endif

export module vulkan_string;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		using vk_string_t = const std::vector<const char*>;
	}
}
