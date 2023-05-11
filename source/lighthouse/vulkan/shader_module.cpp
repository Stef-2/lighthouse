#include "lighthouse/vulkan/shader_module.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/output.hpp"
#include "lighthouse/vulkan/spir_v.hpp"

lh::vulkan::shader_module::shader_module(const logical_device& logical_device,
										 const spir_v& spir_v,
										 const create_info& create_info)
{
	m_object = logical_device->createShaderModule({{}, spir_v.code()});
}
