#pragma once

#include "output.hpp"
#include "static.hpp"

#include <string>
#include <iostream>
#include <filesystem>

namespace lh
{
	class file_system : static_t
	{
	public:
		// find and return engine root directory
		static auto root_path() -> std::filesystem::path;
		// find and return engine data directory
		static auto data_path() -> std::filesystem::path;

		// find the path to a file or directory
		static auto find(std::string_view) -> std::filesystem::path;
	private:
		// once any of these paths are found, they are recorded here for faster retrieval after
		static inline auto m_engine_root_dir = std::pair<std::filesystem::path, const std::filesystem::path::string_type> {{}, L"LightHouse"};
		static inline auto m_data_dir = std::pair<std::filesystem::path, const std::filesystem::path::string_type> {{}, L"data"};
	};
}
