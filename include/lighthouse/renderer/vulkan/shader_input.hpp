#pragma once

namespace lh
{
	namespace vulkan
	{
		struct shader_input
		{
			static inline constexpr auto remove_inactive_inputs = true;

			enum class input_type
			{
				stage_input,
				uniform_buffer
			};

			enum class data_type
			{
				boolean,
				integer,
				unsigned_integer,
				floating,
				structure,
				image,
				sampled_image,
				sampler
			};

			struct struct_member
			{
				data_type m_data_type;
				std::uint8_t m_rows;
				std::uint8_t m_colums;
				std::uint32_t m_array_dimension;
				vk::DeviceSize m_size;
				vk::DeviceSize m_offset;
			};

			uint32_t m_descriptor_set;
			uint32_t m_descriptor_layout;
			uint32_t m_descriptor_binding;

			input_type m_type;
			data_type m_data_type;
			std::uint8_t m_rows;
			std::uint8_t m_columns;
			std::uint32_t m_array_dimension;
			vk::DeviceSize m_size;

			std::vector<struct_member> m_members;
		};
	}
}
