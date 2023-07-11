module;
#pragma once

#include <string_view>

export module file_system;

#if INTELLISENSE
#include <filesystem>
#else
import std.filesystem;
#endif

export namespace lh
{
	namespace file_system
	{
		// find and return engine root directory
		auto root_path() -> std::filesystem::path;

		// find and return engine data directory
		auto data_path() -> std::filesystem::path;

		// find the path to a file or directory
		auto find(std::string_view) -> std::filesystem::path;
	};
}

namespace lh
{
	namespace file_system
	{
		// once any of these paths are found, they are recorded here for faster retrieval after
		std::pair<std::filesystem::path, const std::filesystem::path::string_type> m_engine_root_dir = {{},
																										L"lighthouse"};
		std::pair<std::filesystem::path, const std::filesystem::path::string_type> m_data_dir = {{}, L"data"};
	}
}