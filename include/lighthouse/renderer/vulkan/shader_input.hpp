#pragma once

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class descriptor_set_layout;
		struct vertex_input_description;

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
				integer_16,
				unsigned_integer_16,
				integer_32,
				unsigned_integer_32,
				integer_64,
				unsigned_integer_64,
				float_16,
				float_32,
				float_64,
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

			auto hash() const -> const std::size_t;
			auto translate_format() const -> const vk::Format;

			uint32_t m_descriptor_set;
			uint32_t m_descriptor_location;
			uint32_t m_descriptor_binding;

			input_type m_type;
			data_type m_data_type;
			std::uint8_t m_rows;
			std::uint8_t m_columns;
			std::uint32_t m_array_dimension;
			vk::DeviceSize m_size;

			std::vector<struct_member> m_members;
		};

		class shader_inputs
		{
		public:
			shader_inputs(const std::vector<shader_input>&, const vk::ShaderStageFlags&);

			auto vertex_input_description() -> const vertex_input_description;
			auto descriptor_set_layout() -> const vk::raii::DescriptorSetLayout;
			auto stage() const -> const vk::ShaderStageFlags;

		private:
			std::vector<shader_input> m_shader_inputs;
			vk::ShaderStageFlags m_shader_stage;
		};
	}
}
