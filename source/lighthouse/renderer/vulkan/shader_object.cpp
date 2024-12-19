module;

#if INTELLISENSE
	#include <cstdint>
	#include <ranges>
	#include <iostream>
	#include <numeric>
#endif

module shader_object;

import vulkan_utility;
import output;

namespace lh
{
	namespace vulkan
	{
		shader_pipeline::shader_pipeline() : m_logical_device {}, m_shaders {}, m_pipeline_stages {} {}

		shader_pipeline::shader_pipeline(const logical_device& logical_device,
										 const pipeline_layout& pipeline_layout,
										 const create_info& create_info)
			: m_logical_device {&logical_device}, m_shaders {}, m_pipeline_stages {}
		{
			auto pipeline_create_info = std::vector<vk::ShaderCreateInfoEXT> {};

			pipeline_create_info.reserve(create_info.m_individual_stage_data.size());
			m_shaders.reserve(create_info.m_individual_stage_data.size());
			m_pipeline_stages.reserve(create_info.m_individual_stage_data.size());

			for (auto i = std::size_t {}; i < create_info.m_individual_stage_data.size(); i++)
			{
				const auto& stage_create_info = create_info.m_individual_stage_data[i];

				m_pipeline_stages.emplace_back(stage_create_info.m_shader_stage);

				pipeline_create_info.emplace_back(
					stage_create_info.m_create_flags,
					stage_create_info.m_shader_stage,
					*(std::ranges::find(s_shader_stage_chain, stage_create_info.m_shader_stage) + 1),
					stage_create_info.m_code_type,
					stage_create_info.m_stage_data.m_size,
					stage_create_info.m_stage_data.m_data,
					stage_create_info.m_entrypoint.c_str(),
					static_cast<std::uint32_t>(pipeline_layout.descriptor_set_layouts().size()),
					pipeline_layout.descriptor_set_layouts().data(),
					1,
					&pipeline_layout.push_constant_range());
			}

			m_logical_device->dispatcher()->vkCreateShadersEXT(**logical_device,
															   create_info.m_individual_stage_data.size(),
															   reinterpret_cast<const VkShaderCreateInfoEXT*>(
																   pipeline_create_info.data()),
															   nullptr,
															   reinterpret_cast<VkShaderEXT*>(m_shaders.data()));
		}

		shader_pipeline::shader_pipeline(shader_pipeline&& other)
			: m_logical_device {std::exchange(other.m_logical_device, {})},
			  m_shaders {std::exchange(other.m_shaders, {})},
			  m_pipeline_stages {std::exchange(other.m_pipeline_stages, {})}
		{}

		shader_pipeline& shader_pipeline::operator=(shader_pipeline&& other)
		{
			m_logical_device = std::exchange(other.m_logical_device, {});
			m_shaders = std::exchange(other.m_shaders, {});
			m_pipeline_stages = std::exchange(other.m_pipeline_stages, {});

			return *this;
		}

		auto shader_pipeline::cache_binary_data(const std::vector<std::filesystem::path>& paths) const -> void
		{
			if (paths.size() != m_shaders.size())
			{
				output::warning()
					<< "the number of provided shader object caching paths does not match the number of shader objects";
				return;
			}

			for (auto i = std::size_t {}; i < m_shaders.size(); i++)
			{
				const auto binary_data = utility::shader_object_binary_data(**m_logical_device, m_shaders[i]);

				lh::output::write_file(/*(directory_path /= m_names[i]) /= "bin"*/ paths[i],
									   std::span<const std::byte> {binary_data.cbegin(), binary_data.cend()},
									   std::iostream::out | std::iostream::binary | std::iostream::trunc);
			}
		}

		auto shader_pipeline::stage_count() const -> const std::size_t
		{
			return m_pipeline_stages.size();
		}

		auto shader_pipeline::stages() const -> const std::vector<vk::ShaderStageFlagBits>&
		{
			return m_pipeline_stages;
		}

		auto shader_pipeline::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			command_buffer.bindShadersEXT(m_pipeline_stages, m_shaders);
		}
	}
}
