module;
#pragma once

export module shader_module;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/spir_v.ixx"
#include "lighthouse/string/string.ixx"
#else
import lighthouse_string;
import logical_device;
import spir_v;
import raii_wrapper;
#endif

export namespace lh
{
	namespace vulkan
	{
		class shader_module : public raii_wrapper<vk::raii::ShaderModule>
		{
		public:
			using spir_v_bytecode_t = std::vector<uint32_t>;
			using shader_code_t = string::string_t;

			struct create_info
			{};

			shader_module(const logical_device&, const spir_v&, const create_info& = {});
		};
	}
}