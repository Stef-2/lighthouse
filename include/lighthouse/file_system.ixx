module;

export module file_system;

import data_type;

import std;

namespace lh
{
	namespace file_system
	{
		export enum class directory
		{
			root,
			data,
			shaders,
			shader_binaries,
			meshes,
			images,
			fonts
		};

		export auto initialize() -> void;

		export constexpr auto path(const directory) -> filepath_t;

		// find the path to a file or directory
		export auto find(const std::string_view) -> std::filesystem::path;

		inline const std::map<const directory, const filepath_t::string_type> s_directory_names {
			{directory::root, L"lighthouse"},
			{directory::data, L"data"},
			{directory::shaders, L"shaders"},
			{directory::shader_binaries, L"shader_binaries"},
			{directory::meshes, L"meshes"},
			{directory::images, L"images"},
			{directory::fonts, L"fonts"}};

		inline std::map<const directory, const filepath_t> s_directory_paths {};
	}
}