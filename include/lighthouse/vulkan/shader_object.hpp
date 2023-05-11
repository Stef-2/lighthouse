#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;
		class descriptor_set_layout;
		class spir_v;

		class shader_object : public vk_wrapper<vk::raii::ShaderEXT>
		{
		public:
			struct create_info
			{
				vk::ShaderCreateFlagsEXT m_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
			};

			shader_object(const logical_device&, const spir_v&, const descriptor_set_layout&, const create_info& = {});

		private:
		};

		class shader_objects : public vk_wrapper<vk::raii::ShaderEXTs>
		{
		public:
			struct create_info
			{
				vk::ShaderCreateFlagsEXT m_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
			};

			shader_objects(const logical_device&,
						   const std::vector<std::pair<const spir_v&, const descriptor_set_layout&>>,
						   const create_info& = {});

		private:
		};
	}
}
