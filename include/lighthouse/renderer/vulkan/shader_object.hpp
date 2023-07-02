#pragma once

#include "lighthouse/renderer/vulkan/raii_wrapper.hpp"
#include "lighthouse/renderer/vulkan/shader_input.hpp"

//  forward declarations
namespace std
{
	namespace filesystem
	{
		class path;
	}
}

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;
		class descriptor_set_layout;
		class spir_v;

		class shader_object : public vk_wrapper<vk::raii::ShaderEXT>
		{
		public:
			struct create_info
			{
				vk::ShaderCreateFlagsEXT m_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
			};

			shader_object(const logical_device&, const spir_v&, const descriptor_set_layout&, const create_info& = {});

			auto stage() const -> const vk::ShaderStageFlagBits&;
			auto shader_inputs() const -> const shader_inputs&;
			auto cache_binary_data(const std::filesystem::path&) const -> void;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			vk::ShaderStageFlagBits m_shader_stage;
			vulkan::shader_inputs m_shader_inputs;

			static inline constexpr auto common_shader_entrypoint = "main";
		};
	}
}
