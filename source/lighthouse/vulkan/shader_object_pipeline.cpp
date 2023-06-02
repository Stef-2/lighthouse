#include "lighthouse/vulkan/shader_object_pipeline.hpp"
#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/vulkan/shader_object.hpp"
#include "lighthouse/vulkan/buffer.hpp"
#include "lighthouse/vulkan/spir_v.hpp"

#include <numeric>

lh::vulkan::shader_object_pipeline::shader_object_pipeline(const physical_device& physical_device,
														   const logical_device& logical_device,
														   const memory_allocator& memory_allocator,
														   const std::vector<spir_v>& shaders,
														   const descriptor_set_layout& descriptor_set_layout,
														   const create_info& create_info)
	: m_pipeline_layout {logical_device, {{}, **descriptor_set_layout}},
	  m_shader_objects {},
	  m_descriptor_collection {physical_device, logical_device, memory_allocator, descriptor_set_layout}
{
	m_shader_objects.reserve(shaders.size());

	for (const auto& spir_v : shaders)
		m_shader_objects.emplace_back(logical_device,
									  spir_v,
									  descriptor_set_layout,
									  shader_object::create_info {.m_flags = create_info.m_flags,
																  .m_code_type = create_info.m_code_type});
}

auto lh::vulkan::shader_object_pipeline::shader_objects() const -> const std::vector<shader_object>&
{
	return m_shader_objects;
}

auto lh::vulkan::shader_object_pipeline::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
{
	for (const auto& shader_object : m_shader_objects)
		shader_object.bind(command_buffer);

	m_descriptor_collection.bind(command_buffer, m_pipeline_layout);
}
