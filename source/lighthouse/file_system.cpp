module;

module file_system;

import output;

namespace lh
{
	namespace file_system
	{
		auto initialize() -> void
		{
			for (const auto& dir : std::filesystem::recursive_directory_iterator(std::filesystem::current_path()))
				for (const auto& [directory, name] : s_directory_names)
					if (dir.path().filename() == name) s_directory_paths.emplace(directory, dir);

			if (s_directory_names.size() != s_directory_paths.size())
				lh::output::warning() << "could not find all engine directories, path queries will not work";
		}

		constexpr auto path(const directory directory) -> filepath_t
		{
			return s_directory_paths[directory];
		}

		auto find(const std::string_view name) -> std::filesystem::path
		{
			auto current_dir = s_directory_paths[directory::root];

			for (const auto& dir : std::filesystem::recursive_directory_iterator(current_dir))
				if (dir.path().filename() == name) return dir.path();

			lh::output::warning() << "could not find the specified path: " + string::string_t {name};
			return {};
		}
	}
}
