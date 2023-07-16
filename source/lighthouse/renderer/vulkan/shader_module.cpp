module;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/shader_module.ixx"
#else
module shader_module;
#endif

namespace lh
{
	namespace vulkan
	{
		shader_module::shader_module(const logical_device& logical_device,
									 const spir_v& spir_v,
									 const create_info& create_info)
		{
			m_object = logical_device->createShaderModule({{}, spir_v.code()});
		}
	}
}
