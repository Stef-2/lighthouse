#pragma once

#include "lighthouse/string/string.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		struct shader_input;

		class spir_v
		{
		public:
			using spir_v_bytecode_t = std::vector<uint32_t>;
			using glsl_code_t = string::string_t;

			struct create_info
			{
				vk::ShaderStageFlagBits m_shader_stage = vk::ShaderStageFlagBits::eAll;
			};

			spir_v(const glsl_code_t&, const create_info& = {});
			spir_v(const spir_v_bytecode_t&, const create_info& = {});

			auto reflect_shader_input() const -> std::vector<shader_input>;

			auto code() const -> const spir_v_bytecode_t&;
			auto stage() const -> const vk::ShaderStageFlagBits&;
			auto entrypoint() const -> const string::string_t;

			operator const spir_v_bytecode_t&();

		private:
			auto reflect_shader_entrypoint() const -> string::string_t;

			struct glsl_to_spirv
			{
				static auto translate_shader_stage(const vk::ShaderStageFlagBits&) -> uint32_t;
				static auto translate_shader(const vk::ShaderStageFlagBits&, const glsl_code_t&) -> spir_v_bytecode_t;
			};

			spir_v_bytecode_t m_code;
			string::string_t m_entrypoint;
			vk::ShaderStageFlagBits m_stage;
		};
	}
}