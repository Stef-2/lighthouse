#pragma once

#include "lighthouse/string/string.hpp"

#include <vector>
#include <map>

namespace lh
{
	// forward declarations
	namespace vulkan
	{
		class physical_device;
		class logical_device;
		class memory_allocator;
		class shader_object;
		class spir_v;

		class shader_registry
		{
		public:
			using pipeline_name_t = string::string_t;
			using pipeline_spir_v_code_t = std::vector<spir_v>;

			struct create_info
			{};

			shader_registry(const physical_device&,
							const logical_device&,
							const std::vector<std::pair<pipeline_name_t, pipeline_spir_v_code_t>>,
							const create_info& = {});

		private:
			std::map<pipeline_name_t, std::vector<shader_object>> m_pipelines;
		};
	}
}
