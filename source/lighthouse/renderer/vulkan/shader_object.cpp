module;

#if INTELLISENSE
	#include <cstdint>
	#include <ranges>
	#include <iostream>
	#include <numeric>
#endif

module shader_object;

import output;

namespace lh
{
	namespace vulkan
	{
		shader_object::shader_object(const logical_device& logical_device,
									 const spir_v& spir_v,
									 const pipeline_layout& pipeline_layout,
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
										 static_cast<std::uint32_t>(pipeline_layout.descriptor_set_layouts().size()),
										 pipeline_layout.descriptor_set_layouts().data(),
										 1,
										 &pipeline_layout.push_constant_range()};

			m_object = {*logical_device, shader_create_info};
		}

		auto shader_object::stage() const -> const vk::ShaderStageFlagBits&
		{
			return m_shader_stage;
		}

		auto shader_object::cache_binary_data(const std::filesystem::path& path) const -> void
		{
			const auto binary_data = m_object.getBinaryData();

			lh::output::write_file(path,
								   std::span<const uint8_t> {binary_data.cbegin(), binary_data.cend()},
								   std::iostream::out | std::iostream::binary | std::iostream::trunc);
		}

		auto shader_object::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			command_buffer.bindShadersEXT(m_shader_stage, *m_object);
		}

		// ==========================================================================

		// #pragma optimize("", off)
		shader_pipeline::shader_pipeline(const logical_device& logical_device,
										 const std::vector<spir_v>& pipeline_data,
										 const pipeline_layout& pipeline_layout,
										 const shader_object::create_info& create_info)
			: m_pipeline_stages {}
		{
			auto pipeline_create_info = std::vector<vk::ShaderCreateInfoEXT> {};

			for (const auto& spir_v : pipeline_data)
			{
				m_pipeline_stages.push_back(spir_v.stage());
				/*
				const auto descriptor_set_layouts = pipeline_layout.descriptor_set_layouts();
				const auto& wtf = pipeline_layout.uniform_buffer_set();
				if (*wtf == 0)
				{
					std::cout << *wtf << '\n';
					abort();
				}*/

				pipeline_create_info.emplace_back(create_info.m_modifier_flags,
												  spir_v.stage(),
												  *(std::ranges::find(s_shader_stage_chain, spir_v.stage()) + 1),
												  create_info.m_code_type,
												  spir_v.code().size() * sizeof(spir_v::spir_v_bytecode_t::value_type),
												  spir_v.code().data(),
												  spir_v.entrypoint().c_str(),
												  static_cast<std::uint32_t>(
													  pipeline_layout.descriptor_set_layouts().size()),
												  pipeline_layout.descriptor_set_layouts().data(),
												  1,
												  &pipeline_layout.push_constant_range());
			}

			m_object = {*logical_device, pipeline_create_info};
		}

		auto shader_pipeline::cache_binary_data(const std::vector<std::filesystem::path>& paths) const -> void
		{
			if (m_object.size() != paths.size())
			{
				output::error() << "number of provided cache paths does not match the number of pipeline stages";
				return;
			}

			for (auto i = std::size_t {}; i < m_object.size(); i++)
			{
				const auto binary_data = m_object[i].getBinaryData();

				lh::output::write_file(paths[i],
									   std::span<const uint8_t> {binary_data.cbegin(), binary_data.cend()},
									   std::iostream::out | std::iostream::binary | std::iostream::trunc);
			}
		}

		auto shader_pipeline::stages() const -> const std::vector<vk::ShaderStageFlagBits>&
		{
			return m_pipeline_stages;
		}

		auto shader_pipeline::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			const auto shaders = std::ranges::fold_left(m_object,
														std::vector<vk::ShaderEXT> {},
														[](auto pipeline, const auto& element) {
															pipeline.emplace_back(*element);
															return std::move(pipeline);
														});
			command_buffer.bindShadersEXT(
				m_pipeline_stages,
				shaders); /*
			   command_buffer.getDispatcher()->vkCmdBindShadersEXT(static_cast<VkCommandBuffer>(*command_buffer),
															 m_pipeline_stages.size(),
															 reinterpret_cast<const VkShaderStageFlagBits*>(
																 m_pipeline_stages.data()),
															 reinterpret_cast<const VkShaderEXT*>(m_object.data()));*/
		}
	}
}
