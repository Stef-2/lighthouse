module;

#include "windows.h"

#if INTELLISENSE
#include "lighthouse/operating_system/dynamic_linking.ixx"
#else
module dynamic_linking;
#endif

namespace lh
{
	namespace os
	{
		auto dynamic_linking_directory(const std::filesystem::path& directory_path) -> void
		{
			auto wtf = SetDllDirectoryA(directory_path.generic_string().c_str());
		}
	}
}
