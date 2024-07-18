module;

export module dynamic_linking;

import std;

export namespace lh
{
	namespace os
	{
		 auto dynamic_linking_directory(const std::filesystem::path&) -> void;
	}
}
