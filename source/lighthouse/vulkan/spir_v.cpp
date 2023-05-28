#include "lighthouse/vulkan/spir_v.hpp"
#include "lighthouse/output.hpp"

#include "vulkan/glslang/SPIRV/GlslangToSpv.h"
#include "vulkan/utils/StandAlone.hpp"

#include "vulkan/spirv_cross/spirv_reflect.hpp"

lh::vulkan::spir_v::spir_v(const glsl_code_t& glsl_code, const create_info& create_info)
	: m_stage {create_info.m_shader_stages}
{
	glslang::InitializeProcess();

	m_code = glsl_to_spirv::translate_shader(create_info.m_shader_stages, glsl_code);

	glslang::FinalizeProcess();
	constexpr auto wtf = vk::ShaderStageFlagBits::eVertex;
	reflection();
}

auto lh::vulkan::spir_v::reflection() const -> std::vector<shader_input>
{
	const auto compiler = spirv_cross::CompilerGLSL(m_code);
	const auto resources = compiler.get_shader_resources();

	auto shader_inputs = std::vector<shader_input> {};

	const auto translate_data_type = [](const spirv_cross::SPIRType::BaseType& spirv_type) {
		switch (spirv_type)
		{
			case spirv_cross::SPIRType::BaseType::Boolean: return shader_input::data_type::boolean; break;
			case spirv_cross::SPIRType::BaseType::Int:
			case spirv_cross::SPIRType::BaseType::Int64: return shader_input ::data_type::integer; break;
			case spirv_cross::SPIRType::BaseType::UInt:
			case spirv_cross::SPIRType::BaseType::UInt64: return shader_input::data_type::unsigned_integer; break;
			case spirv_cross::SPIRType::BaseType::Float: return shader_input::data_type::floating; break;
			case spirv_cross::SPIRType::BaseType::Struct: return shader_input::data_type::structure; break;
			case spirv_cross::SPIRType::BaseType::Image: return shader_input::data_type::image; break;
			case spirv_cross::SPIRType::BaseType::SampledImage: return shader_input::data_type::sampled_image; break;
			case spirv_cross::SPIRType::BaseType::Sampler: return shader_input::data_type::sampler; break;
			default: break;
		}
	};

	const auto input =
		[this, &compiler, &translate_data_type](const spirv_cross::Resource& resource,
												const shader_input::input_type& input_type) -> shader_input {
		const auto set = compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet);
		const auto location = compiler.get_decoration(resource.id, spv::Decoration::DecorationLocation);
		const auto binding = compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding);

		const auto data_type = compiler.get_type_from_variable(resource.id).basetype;
		const auto rows = compiler.get_type_from_variable(resource.id).vecsize;
		const auto columns = compiler.get_type_from_variable(resource.id).columns;
		const auto size = compiler.get_type_from_variable(resource.id).width;

		auto input =
			shader_input {set, location, binding, input_type, translate_data_type(data_type), rows, columns, size};

		for (std::size_t i {}; const auto& member : compiler.get_type(resource.base_type_id).member_types)
		{
			const auto data_type = compiler.get_type(member).basetype;
			const auto rows = compiler.get_type(member).vecsize;
			const auto columns = compiler.get_type(member).columns;
			const auto size = compiler.get_type(member).width;
			const auto offset = compiler.type_struct_member_offset(compiler.get_type(member), i);

			input.m_members.emplace_back(translate_data_type(data_type), rows, columns, size, offset);
		}

		return input;
	};

	for (auto& uniform_buffer : resources.uniform_buffers)
	{
		const auto set = compiler.get_decoration(uniform_buffer.id, spv::Decoration::DecorationDescriptorSet);
		const auto location = compiler.get_decoration(uniform_buffer.id, spv::Decoration::DecorationLocation);
		const auto binding = compiler.get_decoration(uniform_buffer.id, spv::Decoration::DecorationBinding);

		const auto data_type = compiler.get_type_from_variable(uniform_buffer.id).basetype;
		const auto rows = compiler.get_type_from_variable(uniform_buffer.id).vecsize;
		const auto size = compiler.get_declared_struct_size(compiler.get_type_from_variable(uniform_buffer.id));

		auto members = compiler.get_type_from_variable(uniform_buffer.id).member_types;

		std::cout << "descriptor size: " << size;
	}

	if (m_stage == vk::ShaderStageFlagBits::eVertex)
		for (auto& vertex_input : resources.stage_inputs)
		{
			const auto set = compiler.get_decoration(vertex_input.id, spv::Decoration::DecorationDescriptorSet);
			const auto binding = compiler.get_decoration(vertex_input.id, spv::Decoration::DecorationBinding);

			std::cout << "\n==================================" << vertex_input.name << " << name\n";
			std::cout << binding << " << binding\n";
			std::cout << compiler.get_type_from_variable(vertex_input.id).width << " << width\n";
			std::cout << compiler.get_type_from_variable(vertex_input.id).vecsize << " << vector size\n";
			std::cout << compiler.get_type_from_variable(vertex_input.id).columns << " << columns\n";
			std::cout << compiler.get_type_from_variable(vertex_input.id).basetype << " << type\n";
		}

	for (auto& builtin : resources.builtin_inputs)
	{
		std::cout << builtin.resource.name << " builtin";
	}

	return shader_inputs;
}

auto lh::vulkan::spir_v::code() const -> const spir_v_bytecode_t&
{
	return m_code;
}

auto lh::vulkan::spir_v::stage() const -> const vk::ShaderStageFlagBits&
{
	return m_stage;
}

lh::vulkan::spir_v::operator const spir_v_bytecode_t&()
{
	return m_code;
}

auto lh::vulkan::spir_v::glsl_to_spirv::translate_shader_stage(const vk::ShaderStageFlagBits& shader_stage) -> uint32_t
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

auto lh::vulkan::spir_v::glsl_to_spirv::translate_shader(const vk::ShaderStageFlagBits& shader_stage,
														 const glsl_code_t& shader_code) -> spir_v_bytecode_t

{
	const auto glsl_shader_stage = static_cast<EShLanguage>(translate_shader_stage(shader_stage));

	const char* shader_string[1] = {shader_code.data()};

	auto glsl_shader = glslang::TShader(glsl_shader_stage);
	glsl_shader.setStrings(shader_string, 1);

	const auto message_types = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

	auto program = glslang::TProgram {};
	program.addShader(&glsl_shader);

	const auto parse = glsl_shader.parse(&DefaultTBuiltInResource, 100, false, message_types);
	const auto link = program.link(message_types);

	if (not parse or not link)
		output::error() << glsl_shader.getInfoLog() << glsl_shader.getInfoDebugLog();

	auto spirv_bytecode = spir_v_bytecode_t {};
	glslang::GlslangToSpv(*program.getIntermediate(glsl_shader_stage), spirv_bytecode);

	return spirv_bytecode;
}
