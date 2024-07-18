module;

export module vulkan_string;

import std;

export namespace lh
{
	namespace vulkan
	{
		using vk_string_t = const std::vector<const char*>;
	}
}
