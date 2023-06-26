#include "lighthouse/renderer/vulkan/shader_registry.hpp"
#include "lighthouse/renderer/vulkan/physical_device.hpp"
#include "lighthouse/renderer/vulkan/logical_device.hpp"
#include "lighthouse/renderer/vulkan/shader_object.hpp"
#include "lighthouse/renderer/vulkan/spir_v.hpp"

lh::vulkan::shader_registry::shader_registry(
	const physical_device& physical_device,
	const logical_device& logical_device,
	const std::vector<std::pair<pipeline_name_t, pipeline_spir_v_code_t>> pipeline_name_code_pairs,
	const create_info& create_info)
	: m_pipelines {}
{
	m_pipelines.
}
