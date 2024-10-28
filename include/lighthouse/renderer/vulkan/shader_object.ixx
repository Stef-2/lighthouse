module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module shader_object;

import lighthouse_utility;
import data_type;
import raii_wrapper;
import logical_device;
import pipeline_layout;
import spir_v;
import shader_input;

import std;

export namespace lh
{
	namespace vulkan
	{/*
		class shader_object : public raii_wrapper<vk::raii::ShaderEXT>
		{
		public:
			using binary_data_t = data_t;

			struct spir_v_create_info
			{
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
				vk::ShaderCreateFlagsEXT m_create_flags = {vk::ShaderCreateFlagBitsEXT::eLinkStage};
				vk::ShaderStageFlagBits m_shader_stage;
				string::string_t m_entrypoint;
			};*/
			/*
			struct binary_create_info
			{
				vk::ShaderCreateFlagsEXT m_create_flags = {vk::ShaderCreateFlagBitsEXT::eLinkStage};
				vk::ShaderStageFlagBits m_shader_stage;
				string::string_t m_entrypoint;
			};*/
		/*
			shader_object(const logical_device&,
						  const spir_v&,
						  const pipeline_layout&,
						  //const string::string_t&,
						  const spir_v_create_info& = {});*/
			/*
			shader_object(const logical_device&,
						  const binary_data_t&,
						  const pipeline_layout&,
						  //const string::string_t&,
						  const binary_create_info& = {});*/
		/*
			auto cache_binary_data(const std::filesystem::path&) const -> void;
			auto stage() const -> const vk::ShaderStageFlagBits&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			vk::ShaderStageFlagBits m_shader_stage;
			//string::string_t m_name;
		};*/
		
		// ====================================================================

		template <std::size_t N = std::dynamic_extent>
		class shader_pipeline : public raii_wrapper<vk::raii::ShaderEXTs>
		{
		public:
			//using pipeline_code_t = std::vector<std::filesystem::path>;

			struct create_info
			{
				struct per_stage_data
				{
					vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
					string::string_t m_entrypoint = spir_v::s_default_entrypoint;
					vk::ShaderStageFlagBits m_shader_stage;
					storage m_data;
				};

				vk::ShaderCreateFlagsEXT m_create_flags = {vk::ShaderCreateFlagBitsEXT::eLinkStage};
				std::vector<per_stage_data> m_per_stage_data;
			};

			using raii_wrapper::raii_wrapper;

			shader_pipeline(const logical_device&,
							//const std::vector<spir_v>&,
							const pipeline_layout&,
							//const std::vector<lh::string::string_t>&,
							const create_info&);
			/*
			shader_pipeline(const logical_device&,
							const std::vector<shader_object::binary_data_t>&,
							const pipeline_layout&,
							//const std::vector<lh::string::string_t>&,
							const std::vector<shader_object::binary_create_info>&);*/

			auto cache_binary_data(const std::filesystem::path&) const -> void;
			auto stages() const -> const std::vector<vk::ShaderStageFlagBits>&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			std::vector<vk::ShaderStageFlagBits> m_pipeline_stages;
			//std::vector<string::string_t> m_names;
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