module;
#pragma once

export module shader_object;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/spir_v.ixx"
#include "lighthouse/renderer/vulkan/shader_input.ixx"
#include "lighthouse/renderer/vulkan/descriptor_set_layout.ixx"
#else
import raii_wrapper;
import descriptor_set_layout;
import logical_device;
import spir_v;
import shader_input;
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
				vk::ShaderCreateFlagsEXT m_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
			};

			shader_object(const logical_device&, const spir_v&, const descriptor_set_layout&, const create_info& = {});

			auto stage() const -> const vk::ShaderStageFlagBits&;
			auto cache_binary_data(const std::filesystem::path&) const -> void;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			vk::ShaderStageFlagBits m_shader_stage;
		};
	}
}
