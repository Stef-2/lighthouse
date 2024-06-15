module;

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

module input_data;

import output;

namespace lh
{
	auto input::read_text_file(const std::filesystem::path& file_path) -> string::string_t
	{
		if (not assert_path_validity(file_path, file_type::text)) return {};

		auto stream = std::ifstream {file_path, std::ios::in};
		auto buffer = std::stringstream {};
		buffer << stream.rdbuf();
		stream.close();

		return buffer.str();
	}

	auto input::read_binary_file(const std::filesystem::path& file_path) -> lh::data_t
	{
		if (not assert_path_validity(file_path, file_type::binary)) return {};

		auto stream = std::ifstream {file_path, std::ios::in | std::ios::binary | std::ios::ate};
		const auto file_size = stream.tellg();
		stream.seekg(std::ios::beg);

		auto buffer = std::vector<std::byte>(file_size);
		stream.read(reinterpret_cast<char*>(buffer.data()), file_size);

		stream.close();

		return buffer;
	}

	input::image_data::image_data() : m_data {}, m_width {}, m_height {}, m_num_color_channels {}, m_data_size {} {}

	input::image_data::~image_data()
	{
		stbi_image_free(m_data);
	}

	input::image_data::image_data(image_data&& other) noexcept
		: m_data {other.m_data},
		  m_width {other.m_width},
		  m_height {other.m_height},
		  m_num_color_channels {other.m_num_color_channels},
		  m_data_size {other.m_data_size}
	{
		other.m_data = {};
		other.m_width = {};
		other.m_height = {};
		other.m_num_color_channels = {};
		other.m_data_size = {};
	}

	input::image_data& input::image_data::operator=(image_data&& other) noexcept
	{
		m_data = other.m_data;
		m_width = other.m_width;
		m_height = other.m_height;
		m_num_color_channels = other.m_num_color_channels;
		m_data_size = other.m_data_size;

		other.m_data = {};
		other.m_width = {};
		other.m_height = {};
		other.m_num_color_channels = {};
		other.m_data_size = {};

		return *this;
	}

	auto input::read_image_file(const std::filesystem::path& file_path) -> const image_data
	{
		if (not assert_path_validity(file_path, file_type::image)) return {};

		constexpr auto texel_format = STBI_rgb_alpha;
		constexpr auto rgba_texel_size = std::uint8_t {4};

		auto image_data = lh::input::image_data {};

		const auto data = stbi_load(file_path.string().c_str(),
									reinterpret_cast<std::int32_t*>(&image_data.m_width),
									reinterpret_cast<std::int32_t*>(&image_data.m_height),
									reinterpret_cast<std::int32_t*>(&image_data.m_num_color_channels),
									texel_format);

		if (not data)
		{
			output::error() << "failed to load texture: " + file_path.string();
			return {};
		}

		image_data.m_data = static_cast<std::byte*>(static_cast<void*>(data));
		image_data.m_data_size = image_data.m_width * image_data.m_height * rgba_texel_size;

		return image_data;
	}

	auto input::assert_path_validity(const std::filesystem::path& file_path, const file_type& file_type) -> bool
	{
		const auto valid_path = not file_path.empty();
		const auto path_exists = std::filesystem::exists(file_path);
		auto valid_extension = false;

		for (const auto& extension : m_valid_file_extensions.at(file_type))
			if (std::strcmp(extension, file_path.extension().string().c_str()) == 0)
			{
				valid_extension = true;
				break;
			}

		if (valid_path and valid_extension and path_exists) return true;

		output::error() << "invalid file path provided: " + file_path.string();
		return false;
	}
}
