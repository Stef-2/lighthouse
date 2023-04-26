#include "lighthouse/vulkan/shader_module.hpp"

#include "vulkan/glslang/SPIRV/GlslangToSpv.h"
// #include "vulkan/utils/StandAlone.hpp"

lh::vulkan::shader_module::shader_module(const logical_device& logical_device,
										 const string::string_t shader_code,
										 const create_info& create_info)
{
	glslang::InitializeProcess();

	std::vector<std::byte> shader_bytecode;

	glslang::FinalizeProcess();
}

auto lh::vulkan::shader_module::glsl_to_spirv::translate_shader_stage(const vk::ShaderStageFlagBits& shader_stage)
	-> uint32_t
{
	switch (shader_stage)
	{
	case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
	case vk::ShaderStageFlagBits::eTessellationControl: return EShLangTessControl;
	case vk::ShaderStageFlagBits::eTessellationEvaluation: return EShLangTessEvaluation;
	case vk::ShaderStageFlagBits::eGeometry: return EShLangGeometry;
	case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
	case vk::ShaderStageFlagBits::eCompute: return EShLangCompute;
	case vk::ShaderStageFlagBits::eRaygenNV: return EShLangRayGenNV;
	case vk::ShaderStageFlagBits::eAnyHitNV: return EShLangAnyHitNV;
	case vk::ShaderStageFlagBits::eClosestHitNV: return EShLangClosestHitNV;
	case vk::ShaderStageFlagBits::eMissNV: return EShLangMissNV;
	case vk::ShaderStageFlagBits::eIntersectionNV: return EShLangIntersectNV;
	case vk::ShaderStageFlagBits::eCallableNV: return EShLangCallableNV;
	case vk::ShaderStageFlagBits::eTaskNV: return EShLangTaskNV;
	case vk::ShaderStageFlagBits::eMeshNV: return EShLangMeshNV;
	default: return EShLangVertex;
	}
}

auto lh::vulkan::shader_module::glsl_to_spirv::translate_shader(const vk::ShaderStageFlagBits& shader_stage,
																const shader_code_t const& shader_code)
	-> spirv_bytecode_t

{
	const auto glsl_shader_stage = static_cast<EShLanguage>(translate_shader_stage(shader_stage));

	const char* const* c = shader_code.data();

	auto glsl_shader = glslang::TShader(glsl_shader_stage);
	glsl_shader.setStrings(c, 1);
}
