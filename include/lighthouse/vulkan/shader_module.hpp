#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"
#include "lighthouse/string/string.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;

		class shader_module : public vk_wrapper<vk::raii::ShaderModule>
		{
		public:
			using spirv_bytecode_t = std::vector<std::byte>;
			using shader_code_t = std::string_view;

			struct create_info
			{
				vk::ShaderStageFlags m_shader_stages = vk::ShaderStageFlagBits::eAll;
			};

			shader_module(const logical_device&, const shader_code_t&, const create_info& = {});

		private:
			class glsl_to_spirv
			{
			public:
				static auto translate_shader_stage(const vk::ShaderStageFlagBits&) -> uint32_t;
				static auto translate_shader(const vk::ShaderStageFlagBits&, const shader_code_t&) -> spirv_bytecode_t;
			};
		};
	}
}
