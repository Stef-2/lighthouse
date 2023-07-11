#pragma once

#if INTELLISENSE
#include "lighthouse/string/string.ixx"
#else
import lighthouse_string;
#endif
#include "lighthouse/renderer/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;
		class spir_v;

		class shader_module : public vk_wrapper<vk::raii::ShaderModule>
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
