#pragma once

#include "lighthouse/string/string.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class shader_inputs;

		class spir_v
		{
		public:
			using spir_v_bytecode_t = std::vector<uint32_t>;
			using glsl_code_t = string::string_t;

			struct create_info
			{
				vk::ShaderStageFlagBits m_shader_stages = vk::ShaderStageFlagBits::eAll;
			};

			spir_v(const glsl_code_t&, const create_info& = {});
			spir_v(const spir_v_bytecode_t&, const create_info& = {});

			auto reflect_shader_input() const -> shader_inputs;

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
