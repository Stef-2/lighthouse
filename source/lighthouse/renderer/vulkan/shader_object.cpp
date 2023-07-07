#include "lighthouse/renderer/vulkan/logical_device.hpp"
#include "lighthouse/renderer/vulkan/shader_object.hpp"
#include "lighthouse/renderer/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/renderer/vulkan/shader_input.hpp"
#include "lighthouse/renderer/vulkan/spir_v.hpp"
#include "lighthouse/output.hpp"

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
															 spir_v.entrypoint().c_str(),
															 1,
															 &**descriptor_set_layout};

	m_object = {*logical_device, shader_create_info};
}

auto lh::vulkan::shader_object::stage() const -> const vk::ShaderStageFlagBits&
{
	return m_shader_stage;
}

auto lh::vulkan::shader_object::cache_binary_data(const std::filesystem::path& path) const -> void
{

	auto binary_data = m_object.getBinaryData();

	lh::output::write_file(path,
						   std::span<uint8_t> {binary_data.begin(), binary_data.end()},
						   std::iostream::out | std::iostream::binary | std::iostream::trunc);
}

auto lh::vulkan::shader_object::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
{
	command_buffer.bindShadersEXT(m_shader_stage, *m_object);
}
