#include "lighthouse/operating_system/dynamic_linking.hpp"

#include "windows.h"

auto lh::os::dynamic_linking_directory(const std::filesystem::path& directory_path) -> void
{
	auto wtf = SetDllDirectoryA(directory_path.generic_string().c_str());
}
