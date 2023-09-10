module;

#if INTELLISENSE
#include <cstdint>
#include <ranges>
#include <iostream>
#endif

module shader_object;

import output;

namespace lh
{
	namespace vulkan
	{
		shader_object::shader_object(const logical_device& logical_device,
									 const spir_v& spir_v,
									 const std::vector<vk::DescriptorSetLayout>& descriptor_set_layouts,
									 const create_info& create_info)
			: m_shader_stage {spir_v.stage()}
		{
			const auto shader_create_info =
				vk::ShaderCreateInfoEXT {create_info.m_modifier_flags,
										 m_shader_stage,
										 *(std::ranges::find(s_shader_stage_chain, m_shader_stage) + 1),
										 create_info.m_code_type,
										 spir_v.code().size() * sizeof(spir_v::spir_v_bytecode_t::value_type),
										 spir_v.code().data(),
										 spir_v.entrypoint().c_str(),
										 static_cast<std::uint32_t>(descriptor_set_layouts.size()),
										 descriptor_set_layouts.data()};

			m_object = {*logical_device, shader_create_info};
		}

		auto shader_object::stage() const -> const vk::ShaderStageFlagBits&
		{
			return m_shader_stage;
		}

		auto shader_object::cache_binary_data(const std::filesystem::path& path) const -> void
		{

			auto binary_data = m_object.getBinaryData();

			lh::output::write_file(path,
								   std::span<uint8_t> {binary_data.begin(), binary_data.end()},
								   std::iostream::out | std::iostream::binary | std::iostream::trunc);
		}

		auto shader_object::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			command_buffer.bindShadersEXT(m_shader_stage, *m_object);
		}
	}
}
