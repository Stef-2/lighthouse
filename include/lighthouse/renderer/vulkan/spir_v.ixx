module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <utility>
#endif

export module spir_v;

import lighthouse_string;
import file_system;
import shader_input;
import vertex_input_description;

#if not INTELLISENSE
import vulkan;
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		class spir_v
		{
		public:
			using spir_v_bytecode_t = std::vector<uint32_t>;
			using glsl_code_t = string::string_t;

			struct create_info
			{};

			spir_v(const glsl_code_t&, const create_info& = {});

			auto reflect_shader_input() const -> std::vector<shader_input>;

			auto code() const -> const spir_v_bytecode_t&;
			auto stage() const -> const vk::ShaderStageFlagBits&;
			auto entrypoint() const -> const string::string_t&;

			operator const spir_v_bytecode_t&();
			operator const spir_v_bytecode_t&() const;

		private:
			auto reflect_shader_entrypoint_and_stage() const -> const std::pair<string::string_t, vk::ShaderStageFlagBits>;

			struct glsl_to_spirv
			{
				static auto translate_shader(const glsl_code_t&) -> spir_v_bytecode_t;
			};

			spir_v_bytecode_t m_code;
			string::string_t m_entrypoint;
			vk::ShaderStageFlagBits m_stage;
		};
	}
}

namespace lh
{
	namespace vulkan
	{
		constexpr auto remove_inactive_inputs = true;
	}
}