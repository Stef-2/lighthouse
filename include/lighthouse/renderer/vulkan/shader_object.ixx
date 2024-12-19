module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module shader_object;

import lighthouse_utility;
import lighthouse_string;
import data_type;
import logical_device;
import pipeline_layout;
import spir_v;
import shader_input;

import std;
import vulkan_hpp;

export namespace lh
{
	namespace vulkan
	{
		//template <std::size_t N = std::dynamic_extent>
		class shader_pipeline
		{
		public:
			struct create_info
			{
				struct individual_stage_data
				{
					vk::ShaderCreateFlagsEXT m_create_flags = {vk::ShaderCreateFlagBitsEXT::eLinkStage};
					vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
					vk::ShaderStageFlagBits m_shader_stage;
					data_range m_stage_data;
					string::string_t m_entrypoint = spir_v::s_default_entrypoint;
				};

				std::vector<individual_stage_data> m_individual_stage_data;
			};

			shader_pipeline();
			shader_pipeline(const logical_device&,
							const pipeline_layout&,
							const create_info&);

			// disallow copy construction
			shader_pipeline(const shader_pipeline&) = delete;
			shader_pipeline& operator=(const shader_pipeline&) = delete;
			shader_pipeline(shader_pipeline&&);
			shader_pipeline& operator=(shader_pipeline&&);

			auto cache_binary_data(const std::vector<filepath_t>&) const -> void;
			auto stage_count() const -> const std::size_t;
			auto stages() const -> const std::vector<vk::ShaderStageFlagBits>&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			const logical_device* m_logical_device;
			std::vector<vk::ShaderEXT> m_shaders;
			std::vector<vk::ShaderStageFlagBits> m_pipeline_stages;
		};
	}
}

module :private;

namespace lh
{
	namespace vulkan
	{
		constexpr auto s_shader_stage_chain = std::array<vk::ShaderStageFlagBits, 3> {
			vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment, {}};
	}
}