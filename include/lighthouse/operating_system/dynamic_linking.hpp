#pragma once

#include <filesystem>

namespace lh
{
	namespace os
	{
		 auto dynamic_linking_directory(const std::filesystem::path&) -> void;

	}
}
