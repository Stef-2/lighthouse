module;

module dynamic_rendering_state;

namespace lh
{
	namespace vulkan
	{
		dynamic_rendering_state::dynamic_rendering_state(const create_info& create_info)
			: m_state {std::move(create_info)}
		{}

		auto dynamic_rendering_state::state() -> create_info&
		{
			return m_state;
		}

		auto dynamic_rendering_state::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			command_buffer.setViewportWithCountEXT(m_state.m_viewport);
			command_buffer.setScissorWithCountEXT(m_state.m_scissor);
			command_buffer.setCullModeEXT(m_state.m_cull_mode);
			command_buffer.setFrontFaceEXT(m_state.m_front_face);
			command_buffer.setDepthTestEnableEXT(m_state.m_depth_testing);
			command_buffer.setDepthWriteEnableEXT(m_state.m_depth_writing);
			command_buffer.setDepthCompareOpEXT(m_state.m_depth_compare_operation);
			command_buffer.setPrimitiveTopologyEXT(m_state.m_primitive_topology);
			command_buffer.setLineWidth(m_state.m_line_width);
			command_buffer.setRasterizerDiscardEnableEXT(m_state.m_rasterizer_discarding);
			command_buffer.setPolygonModeEXT(m_state.m_polygon_mode);
			command_buffer.setRasterizationSamplesEXT(m_state.m_sample_count);
			command_buffer.setSampleMaskEXT(m_state.m_sample_mask.first, m_state.m_sample_mask.second);
			command_buffer.setAlphaToCoverageEnableEXT(m_state.m_alpha_to_coverage);
			command_buffer.setDepthBiasEnable(m_state.m_depth_biasing);
			command_buffer.setStencilTestEnable(m_state.stencil_testing);
			command_buffer.setPrimitiveRestartEnable(m_state.primitive_restarting);
			command_buffer.setColorBlendEnableEXT(m_state.m_color_blend_enabling.first,
												  m_state.m_color_blend_enabling.second);
			command_buffer.setColorBlendEquationEXT(m_state.m_color_blend_equation.first,
													m_state.m_color_blend_equation.second);
			command_buffer.setColorWriteMaskEXT(m_state.m_color_write_mask.first, m_state.m_color_write_mask.second);
		}
	}
}
