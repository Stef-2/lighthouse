#pragma once

#include "lighthouse/string/string.hpp"

namespace lh
{
	namespace vulkan
	{
		class spir_v
		{
		public:
			using spir_v_bytecode_t = std::vector<uint32_t>;
			using glsl_code_t = string::string_t;

			struct shader_input
			{
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
					uint32_t m_rows;
					uint32_t m_colums;
					uint32_t m_size;
					uint32_t m_offset;
				};

				uint32_t m_descriptor_set;
				uint32_t m_descriptor_layout;
				uint32_t m_descriptor_binding;

				input_type m_type;
				data_type m_data_type;
				uint32_t m_rows;
				uint32_t m_columns;
				uint32_t m_size;

				std::vector<struct_member> m_members;
			};

			struct create_info
			{
				vk::ShaderStageFlagBits m_shader_stages = vk::ShaderStageFlagBits::eAll;
			};

			spir_v(const glsl_code_t&, const create_info& = {});

			auto reflection() const -> std::vector<shader_input>;

			auto code() const -> const spir_v_bytecode_t&;
			auto stage() const -> const vk::ShaderStageFlagBits&;

			operator const spir_v_bytecode_t&();

		private:
			struct glsl_to_spirv
			{
				static auto translate_shader_stage(const vk::ShaderStageFlagBits&) -> uint32_t;
				static auto translate_shader(const vk::ShaderStageFlagBits&, const glsl_code_t&) -> spir_v_bytecode_t;
			};

			spir_v_bytecode_t m_code;
			vk::ShaderStageFlagBits m_stage;
		};
	}
}
