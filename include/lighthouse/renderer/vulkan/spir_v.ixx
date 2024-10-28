module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif

export module spir_v;

import lighthouse_string;
import data_type;
import shader_input;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import std;

export namespace lh
{
	namespace vulkan
	{
		class spir_v
		{
		public:
			using glsl_code_t = string::string_t;
			using spir_v_code_t = std::vector<std::uint32_t>;

			static constexpr inline auto s_default_entrypoint = "main";
			static constexpr inline auto s_spir_v_byteword = 4;

			struct create_info
			{
				// optional entrypoint and shader stage info
				// if not provided, they will be reflected instead
				std::optional<string::string_t> m_entrypoint = s_default_entrypoint;
				std::optional<vk::ShaderStageFlagBits> m_stage = {};
			};

			spir_v(const glsl_code_t&, const create_info& = {});
			spir_v(const spir_v_code_t&, const create_info& = {});

			auto reflect_shader_input() const -> std::vector<shader_input>;

			auto code() const -> const spir_v_code_t&;
			auto byte_span() const -> const std::span<std::byte>;
			//auto stage() const -> const vk::ShaderStageFlagBits;
			//auto entrypoint() const -> const string::string_t&;

			auto cache_binary_data(const filepath_t&) const -> void;

		private:
			auto reflect_shader_entrypoint_and_stage() const -> const std::pair<string::string_t, vk::ShaderStageFlagBits>;

			struct glsl_to_spirv
			{
				static auto translate_shader(const glsl_code_t&) -> spir_v_code_t;
			};

			spir_v_code_t m_code;
			//string::string_t m_entrypoint;
			//vk::ShaderStageFlagBits m_stage;
		};
	}
}

namespace lh
{
	namespace vulkan
	{
		constexpr auto remove_inactive_inputs = false;
	}
}