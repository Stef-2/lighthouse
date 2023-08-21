module;

#if INTELLISENSE
#include <concepts>
#endif

export module vulkan_utility;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		namespace utility
		{

			template <typename T>
				requires std::integral<T>
			auto aligned_size(T value, T alignment)
			{
				return (value + alignment - 1) & ~(alignment - 1);
			}

		}
	}
}
