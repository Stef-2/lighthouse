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
									 const string::string_t& name,
									 const spir_v_create_info& create_info)
			: m_shader_stage {spir_v.stage()}, m_name {name}
		{
			const auto shader_create_info =
				vk::ShaderCreateInfoEXT {create_info.m_create_flags,
										 m_shader_stage,
										 *(std::ranges::find(s_shader_stage_chain, m_shader_stage) + 1),
										 vk::ShaderCodeTypeEXT::eSpirv,
										 spir_v.code().size() * sizeof(spir_v::spir_v_code_t::value_type),
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

		shader_object::shader_object(const logical_device& logical_device,
									 const binary_data_t& binary_data,
									 const pipeline_layout& pipeline_layout,
									 const string::string_t& name,
									 const binary_create_info& create_info)
			: m_shader_stage {create_info.m_shader_stage}, m_name {name}
		{
			const auto shader_create_info =
				vk::ShaderCreateInfoEXT {create_info.m_create_flags,
										 m_shader_stage,
										 *(std::ranges::find(s_shader_stage_chain, m_shader_stage) + 1),
										 vk::ShaderCodeTypeEXT::eBinary,
										 binary_data.size() * sizeof(binary_data_t::value_type),
										 binary_data.data(),
										 create_info.m_entrypoint.c_str(),
										 static_cast<std::uint32_t>(pipeline_layout.descriptor_set_layouts().size()),
										 pipeline_layout.descriptor_set_layouts().data(),
										 1,
										 &pipeline_layout.push_constant_range()};

			m_object = {*logical_device, shader_create_info};
		}

		auto shader_object::cache_binary_data(const std::filesystem::path& path) const -> void
		{
			const auto binary_data = m_object.getBinaryData();

			lh::output::write_file(/*(directory_path /= m_name) /= "bin"*/ path,
								   std::span<const uint8_t> {binary_data.cbegin(), binary_data.cend()},
								   std::iostream::out | std::iostream::binary | std::iostream::trunc);
		}

		auto shader_object::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			command_buffer.bindShadersEXT(m_shader_stage, *m_object);
		}

		// ==========================================================================

		shader_pipeline::shader_pipeline(const logical_device& logical_device,
										 const std::vector<spir_v>& spir_v,
										 const pipeline_layout& pipeline_layout,
										 const std::vector<lh::string::string_t>& names,
										 const std::vector<shader_object::spir_v_create_info>& create_infos)
			: m_pipeline_stages {}, m_names {names}
		{
			if (spir_v.size() != names.size() and names.size() != create_infos.size())
			{
				output::error() << "parameter sizes differ for shader_pipeline:";
				return;
			}

			auto pipeline_create_info = std::vector<vk::ShaderCreateInfoEXT> {};
			m_pipeline_stages.reserve(spir_v.size());

			for (auto i = std::size_t {}; i < spir_v.size(); i++)
			{
				m_pipeline_stages.emplace_back(spir_v[i].stage());

				pipeline_create_info.emplace_back(create_infos[i].m_create_flags,
												  spir_v[i].stage(),
												  *(std::ranges::find(s_shader_stage_chain, spir_v[i].stage()) + 1),
												  vk::ShaderCodeTypeEXT::eSpirv,
												  spir_v[i].code().size() * sizeof(spir_v::spir_v_code_t::value_type),
												  spir_v[i].code().data(),
												  spir_v[i].entrypoint().c_str(),
												  static_cast<std::uint32_t>(
													  pipeline_layout.descriptor_set_layouts().size()),
												  pipeline_layout.descriptor_set_layouts().data(),
												  1,
												  &pipeline_layout.push_constant_range());
			}

			m_object = {*logical_device, pipeline_create_info};
		}

		shader_pipeline::shader_pipeline(const logical_device& logical_device,
										 const std::vector<shader_object::binary_data_t>& binary_data,
										 const pipeline_layout& pipeline_layout,
										 const std::vector<lh::string::string_t>& names,
										 const std::vector<shader_object::binary_create_info>& create_infos)
			: m_pipeline_stages {}, m_names {names}
		{
			if (binary_data.size() != names.size() and names.size() != create_infos.size())
			{
				output::error() << "parameter sizes differ for shader_pipeline:";
				return;
			}

			auto pipeline_create_info = std::vector<vk::ShaderCreateInfoEXT> {};
			m_pipeline_stages.reserve(binary_data.size());

			for (auto i = std::size_t {}; i < binary_data.size(); i++)
			{
				m_pipeline_stages.emplace_back(create_infos[i].m_shader_stage);

				pipeline_create_info.emplace_back(
					create_infos[i].m_create_flags,
					create_infos[i].m_shader_stage,
					*(std::ranges::find(s_shader_stage_chain, create_infos[i].m_shader_stage) + 1),
					vk::ShaderCodeTypeEXT::eBinary,
					binary_data[i].size() * sizeof(shader_object::binary_data_t::value_type),
					binary_data[i].data(),
					create_infos[i].m_entrypoint.c_str(),
					static_cast<std::uint32_t>(pipeline_layout.descriptor_set_layouts().size()),
					pipeline_layout.descriptor_set_layouts().data(),
					1,
					&pipeline_layout.push_constant_range());
			}

			m_object = {*logical_device, pipeline_create_info};
		}

		auto shader_pipeline::cache_binary_data(const std::filesystem::path& path) const -> void
		{
			for (auto i = std::size_t {}; i < m_object.size(); i++)
			{
				const auto binary_data = m_object[i].getBinaryData();

				lh::output::write_file(/*(directory_path /= m_names[i]) /= "bin"*/ path,
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

			command_buffer.bindShadersEXT(m_pipeline_stages, shaders);
		}
	}
}
