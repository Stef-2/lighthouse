module;
#pragma once

#include <vector>

export module vulkan_string;

export namespace lh
{
	namespace vulkan
	{
		using vk_string_t = const std::vector<const char*>;
	}
}
