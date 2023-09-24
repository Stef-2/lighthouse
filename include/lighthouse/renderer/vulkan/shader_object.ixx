module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <filesystem>
#include <utility>
#endif

export module shader_object;

import raii_wrapper;
import logical_device;
import spir_v;
import shader_input;

#if not INTELLISENSE
import std.filesystem;
#endif

export namespace lh
{
	namespace vulkan
	{
		class shader_object : public raii_wrapper<vk::raii::ShaderEXT>
		{
		public:
			struct create_info
			{
				vk::ShaderCreateFlagsEXT m_modifier_flags = {};
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
			};

			shader_object(
				const logical_device&,
				const spir_v&,
				const std::vector<vk::DescriptorSetLayout>&, const create_info& = {});

			auto cache_binary_data(const std::filesystem::path&) const -> void;
			auto stage() const -> const vk::ShaderStageFlagBits&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			vk::ShaderStageFlagBits m_shader_stage;
		};

		// ====================================================================

		class shader_pipeline : public raii_wrapper<vk::raii::ShaderEXTs>
		{
		public:
			using raii_wrapper::raii_wrapper;
			using individual_stage_data_t = std::pair<const spir_v&, const std::vector<vk::DescriptorSetLayout>>;

			shader_pipeline(const logical_device&,
							const std::vector<individual_stage_data_t>&,
							const shader_object::create_info& = {.m_modifier_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage});

			auto cache_binary_data(const std::vector<std::filesystem::path>&) const -> void;
			auto stages() const -> const std::vector<vk::ShaderStageFlagBits>&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
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