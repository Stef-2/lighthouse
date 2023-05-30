#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/shader_object.hpp"
#include "lighthouse/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/vulkan/spir_v.hpp"

#include <iostream>

lh::vulkan::shader_object::shader_object(const logical_device& logical_device,
										 const spir_v& spir_v,
										 const descriptor_set_layout& descriptor_set_layout,
										 const create_info& create_info)
	: m_shader_stage {spir_v.stage()}
{
	const auto shader_create_info = vk::ShaderCreateInfoEXT {create_info.m_flags,
															 m_shader_stage,
															 {},
															 create_info.m_code_type,
															 spir_v.code().size() *
																 sizeof(spir_v::spir_v_bytecode_t::value_type),
															 spir_v.code().data(),
															 "main",
															 1,
															 &(**descriptor_set_layout)};

	m_object = {*logical_device, shader_create_info};
}

auto lh::vulkan::shader_object::stage() const -> const vk::ShaderStageFlagBits&
{
	return m_shader_stage;
}

auto lh::vulkan::shader_object::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
{
	command_buffer.bindShadersEXT(m_shader_stage, *m_object);
}
