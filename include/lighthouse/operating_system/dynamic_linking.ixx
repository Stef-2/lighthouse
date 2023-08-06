module;
#pragma once

export module dynamic_linking;

#if INTELLISENSE
import std;
#else
import std.filesystem;
#endif

export namespace lh
{
	namespace os
	{
		 auto dynamic_linking_directory(const std::filesystem::path&) -> void;
	}
}
