module;

export module file_system;

#if INTELLISENSE
#include <filesystem>
#else
import std.filesystem;
import std.core;
#endif


namespace lh
{
	namespace file_system
	{
		// find and return engine root directory
		export auto root_path() -> std::filesystem::path;

		// find and return engine data directory
		export auto data_path() -> std::filesystem::path;

		// find the path to a file or directory
		export auto find(std::string_view) -> std::filesystem::path;

		// once any of these paths are found, they are recorded here for faster retrieval after
		std::pair<std::filesystem::path, const std::filesystem::path::string_type> n_engine_root_dir = {{},
																										L"lighthouse"};
		std::pair<std::filesystem::path, const std::filesystem::path::string_type> n_data_dir = {{}, L"data"};
	}
}