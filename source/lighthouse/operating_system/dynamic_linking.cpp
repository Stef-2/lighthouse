module;

#include "windows.h"

module dynamic_linking;

namespace lh
{
	namespace os
	{
		auto dynamic_linking_directory(const std::filesystem::path& directory_path) -> void
		{
			SetDllDirectoryA(directory_path.generic_string().c_str());
		}
	}
}
