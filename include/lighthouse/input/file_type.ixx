module;

export module file_type;

import std;

export namespace lh
{
	enum class file_type
	{
		text,
		binary,
		image,
		font,
		scene
	};

	inline const auto m_valid_file_extensions = std::map<file_type, const std::vector<const char*>> {
		{file_type::text, {".txt", ".vert", ".frag", ".glsl", ".h", ".hpp"}},
		{file_type::image, {".png"}},
		{file_type::font, {".ttf"}}};
}
