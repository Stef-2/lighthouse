module;

module shader_object_pipeline;

namespace lh
{
	namespace vulkan
	{
		shader_object_pipeline::shader_object_pipeline(const physical_device& physical_device,
													   const logical_device& logical_device,
													   const memory_allocator& memory_allocator,
													   const std::vector<spir_v>& shaders,
													   const create_info& create_info)
			: m_pipeline_layout {nullptr}, m_shader_objects {}, m_descriptor_set_layout {}
		{
			m_shader_objects.reserve(shaders.size());

			for (const auto& spir_v : shaders)
			{
				// const auto shader_inputs = spir_v.reflect_shader_input();
				/*
				const auto bindings = std::ranges::fold_left(
					shader_inputs, std::vector<descriptor_set_layout::binding> {}, [](auto bindings, const auto&
				shader_input) { if (shader_input.m_type == shader_input::input_type::uniform_buffer)
							bindings.emplace_back(shader_input.m_descriptor_binding,
				vk::DescriptorType::eUniformBuffer); return std::move(bindings);
					});

				m_descriptor_set_layout = std::make_unique<vulkan::descriptor_set_layout>(logical_device, bindings);
				*/
				/*
				m_shader_objects.emplace_back(logical_device,
											  spir_v,
											  *m_descriptor_set_layout,
											  shader_object::create_info {.m_modifier_flags = create_info.m_modifier_flags,
																		  .m_code_type = create_info.m_code_type});*/
			}

			// m_pipeline_layout = std::move(vk::raii::PipelineLayout {logical_device, {{},
			//  ***m_descriptor_set_layout}});
		}

		auto shader_object_pipeline::shader_objects() const -> const std::vector<shader_object>&
		{
			return m_shader_objects;
		}

		auto shader_object_pipeline::pipeline_layout() const -> const vk::raii::PipelineLayout&
		{
			return m_pipeline_layout;
		}

		auto shader_object_pipeline::descriptor_set_layout() const -> const vulkan::descriptor_set_layout&
		{
			return m_descriptor_set_layout;
		}
		/*
		auto shader_object_pipeline::descriptor_buffer() const -> const vulkan::descriptor_buffer&
		{
			return m_resource_descriptor_buffer;
		}*/

		auto shader_object_pipeline::vertex_input_description() const -> const vulkan::vertex_input_description&
		{
			return m_vertex_input_description;
		}

		auto shader_object_pipeline::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			for (const auto& shader_object : m_shader_objects)
				shader_object.bind(command_buffer);
		}
	}
}
