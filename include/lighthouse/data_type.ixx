module;

export module data_type;

import std;

export namespace lh
{
	using float32_t = float;
	using float64_t = double;

	// floating point values which are expected to be in the [0.0, 1.0] range
	using float01_t = float32_t;

	// raw binary data
	using data_t = std::vector<std::byte>;

	using filepath_t = std::filesystem::path;
}