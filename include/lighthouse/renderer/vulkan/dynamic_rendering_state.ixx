module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module dynamic_rendering_state;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import data_type;

import std;

export namespace lh
{
	namespace vulkan
	{
		class dynamic_rendering_state
		{
		public:
			struct create_info
			{
				vk::Viewport m_viewport = {};
				vk::Rect2D m_scissor = {};
				vk::CullModeFlags m_cull_mode = vk::CullModeFlagBits::eBack;
				vk::FrontFace m_front_face = vk::FrontFace::eClockwise;
				bool m_depth_testing = true;
				bool m_depth_writing = true;
				vk::CompareOp m_depth_compare_operation = vk::CompareOp::eLessOrEqual;
				vk::PrimitiveTopology m_primitive_topology = vk::PrimitiveTopology::eTriangleList;
				bool m_rasterizer_discarding = false;
				vk::PolygonMode m_polygon_mode = vk::PolygonMode::eFill;
				float32_t m_line_width = 1.0f;
				vk::SampleCountFlagBits m_sample_count = vk::SampleCountFlagBits::e1;
				std::pair<vk::SampleCountFlagBits, std::vector<std::uint32_t>> m_sample_mask = {
					vk::SampleCountFlagBits::e1, {1}};
				bool m_alpha_to_coverage = false;
				bool m_depth_biasing = false;
				bool stencil_testing = false;
				bool primitive_restarting = false;
				std::pair<std::uint32_t, std::vector<std::uint32_t>> m_color_blend_enabling = {0, {true, false}};
				std::pair<std::uint32_t, std::vector<vk::ColorBlendEquationEXT>> m_color_blend_equation = {
					0,
					{vk::ColorBlendEquationEXT {vk::BlendFactor::eSrcAlpha,
												vk::BlendFactor::eOneMinusSrcAlpha,
												vk::BlendOp::eAdd,
												vk::BlendFactor::eOne,
												vk::BlendFactor::eZero,
												vk::BlendOp::eAdd}}};
				std::pair<std::uint32_t, std::vector<vk::ColorComponentFlags>> m_color_write_mask = {
					0,
					{vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
					 vk::ColorComponentFlagBits::eA}};
			};

			dynamic_rendering_state(const create_info& = {});

			auto state() -> create_info&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			create_info m_state;
		};
	}
}