#include "lighthouse/vulkan/pipeline.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/memory_allocator.hpp"
#include "lighthouse/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/vulkan/spir_v.hpp"
#include "lighthouse/vulkan/shader_object.hpp"
#include "lighthouse/vulkan/descriptor_collection.hpp"
#include "lighthouse/vulkan/buffer.hpp"

#include <numeric>

lh::vulkan::pipeline::pipeline(const physical_device& physical_device,
							   const logical_device& logical_device,
							   const memory_allocator& memory_allocator,
							   const std::vector<std::pair<spir_v, descriptor_set_layout>>& shader_data,
							   const create_info& create_info)
	: m_pipeline_layout {nullptr}, m_shaders {}, m_descriptor_collections {}
{
	const auto descriptor_set_layouts = std::accumulate(shader_data.begin(),
														shader_data.end(),
														std::vector<vk::DescriptorSetLayout> {},
														[](auto layout, const auto& d) {
															layout.emplace_back(**d.second);
															return std::move(layout);
														});
	m_pipeline_layout = std::move(vk::raii::PipelineLayout {logical_device, {{}, descriptor_set_layouts}});

	m_shaders.reserve(shader_data.size());
	m_descriptor_collections.reserve(shader_data.size());

	for (const auto& data : shader_data)
	{
		m_shaders.emplace_back(logical_device, data.first, data.second);
		m_descriptor_collections.emplace_back(physical_device, logical_device, data.second, memory_allocator);
	}
}

auto lh::vulkan::pipeline::bind() const -> void {}
