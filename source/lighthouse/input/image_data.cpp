module;

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

module image_data;

import output;

namespace lh
{
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
}
