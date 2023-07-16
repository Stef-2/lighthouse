module;

#include "vulkan/glslang/SPIRV/GlslangToSpv.h"
#include "vulkan/utils/StandAlone.hpp"

#include "vulkan/spirv_cross/spirv_reflect.hpp"

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/spir_v.ixx"
#else
module spir_v;
#endif

import output;

namespace
{
	auto translate_data_type(const spirv_cross::SPIRType::BaseType& spirv_type)
	{
		using return_type = lh::vulkan::shader_input::data_type;

		switch (spirv_type)
		{
			case spirv_cross::SPIRType::BaseType::Boolean: return return_type::boolean;

			case spirv_cross::SPIRType::BaseType::Short: return return_type::integer_16;
			case spirv_cross::SPIRType::BaseType::UShort: return return_type::unsigned_integer_16;
			case spirv_cross::SPIRType::BaseType::Int: return return_type::integer_32;
			case spirv_cross::SPIRType::BaseType::UInt: return return_type::unsigned_integer_32;
			case spirv_cross::SPIRType::BaseType::Int64: return return_type::integer_64;
			case spirv_cross::SPIRType::BaseType::UInt64: return return_type::unsigned_integer_64;

			case spirv_cross::SPIRType::BaseType::Half: return return_type::float_16;
			case spirv_cross::SPIRType::BaseType::Float: return return_type::float_32;
			case spirv_cross::SPIRType::BaseType::Double: return return_type::float_64;

			case spirv_cross::SPIRType::BaseType::Struct: return return_type::structure;
			case spirv_cross::SPIRType::BaseType::Image: return return_type::image;
			case spirv_cross::SPIRType::BaseType::SampledImage: return return_type::sampled_image;
			case spirv_cross::SPIRType::BaseType::Sampler: return return_type::sampler;

			default: lh::output::warning() << "unrecognized spir_v base type: " + spirv_type; break;
		}
	}

	auto create_shader_input(const spirv_cross::CompilerGLSL& compiler,
							 const spirv_cross::Resource& resource,
							 const lh::vulkan::shader_input::input_type& input_type,
							 const vk::ShaderStageFlags& shader_stage)

	{
		constexpr auto byte_divisor = std::uint8_t {8};

		const auto set = compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet);
		const auto location = compiler.get_decoration(resource.id, spv::Decoration::DecorationLocation);
		const auto binding = compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding);

		const auto data_type = compiler.get_type_from_variable(resource.id).basetype;
		const auto rows = compiler.get_type_from_variable(resource.id).vecsize;
		const auto columns = compiler.get_type_from_variable(resource.id).columns;
		const auto array_dimension = compiler.get_type(resource.type_id).array.empty()
										 ? 1
										 : compiler.get_type(resource.type_id).array[0];
		const auto size = compiler.get_type(resource.base_type_id).member_types.empty()
							  ? compiler.get_type_from_variable(resource.id).width / byte_divisor * rows * columns
							  : compiler.get_declared_struct_size(compiler.get_type(resource.base_type_id));

		auto input = lh::vulkan::shader_input {set,
											   location,
											   binding,
											   input_type,
											   translate_data_type(data_type),
											   static_cast<std::uint8_t>(rows),
											   static_cast<std::uint8_t>(columns),
											   array_dimension,
											   size};

		for (std::size_t i {}; const auto& member : compiler.get_type(resource.base_type_id).member_types)
		{
			const auto member_data_type = compiler.get_type(member).basetype;
			const auto member_rows = compiler.get_type(member).vecsize;
			const auto member_columns = compiler.get_type(member).columns;
			const auto member_array_dimension = compiler.get_type(member).array.empty()
													? 1
													: compiler.get_type(member).array[0];
			const auto member_size = compiler.get_declared_struct_member_size(compiler.get_type(resource.base_type_id),
																			  i);
			const auto member_offset = compiler.type_struct_member_offset(compiler.get_type(resource.base_type_id), i);

			input.m_members.emplace_back(translate_data_type(member_data_type),
										 member_rows,
										 member_columns,
										 member_array_dimension,
										 member_size,
										 member_offset);
			i++;
		}

		return input;
	}
}

namespace lh
{
	namespace vulkan
	{
		spir_v::spir_v(const spir_v_bytecode_t& spir_v_code, const create_info& create_info)
			: m_code {spir_v_code}, m_stage {create_info.m_shader_stage}
		{}

		spir_v::spir_v(const glsl_code_t& glsl_code, const create_info& create_info)
			: m_code {}, m_entrypoint {}, m_stage {create_info.m_shader_stage}
		{
			glslang::InitializeProcess();

			m_code = glsl_to_spirv::translate_shader(create_info.m_shader_stage, glsl_code);

			glslang::FinalizeProcess();

			m_entrypoint = reflect_shader_entrypoint();
		}

		auto spir_v::reflect_shader_input() const -> std::vector<shader_input>
		{
			auto compiler = std::make_unique<spirv_cross::CompilerGLSL>(m_code);
			auto resources = compiler->get_shader_resources();

			if constexpr (shader_input::remove_inactive_inputs)
			{
				auto interface_variables = compiler->get_active_interface_variables();
				resources = compiler->get_shader_resources(interface_variables);

				compiler->set_enabled_interface_variables(std::move(interface_variables));
			}

			auto shader_inputs = std::vector<shader_input> {};
			shader_inputs.reserve(resources.stage_inputs.size() + resources.uniform_buffers.size());

			for (const auto& resource : resources.stage_inputs)
				shader_inputs.emplace_back(
					create_shader_input(*compiler, resource, shader_input::input_type::stage_input, m_stage));

			for (const auto& resource : resources.uniform_buffers)
				shader_inputs.emplace_back(
					create_shader_input(*compiler, resource, shader_input::input_type::uniform_buffer, m_stage));

			std::ranges::sort(shader_inputs, [](const auto& x, const auto& y) {
				switch (x.m_type)
				{
					case shader_input::input_type::stage_input:
						return (x.m_descriptor_location < y.m_descriptor_location);
					case shader_input::input_type::uniform_buffer:
						return (x.m_descriptor_binding < y.m_descriptor_binding);
					default: break;
				}
			});

			return shader_inputs;
		}

		auto spir_v::reflect_shader_entrypoint() const -> string::string_t
		{
			const auto compiler = spirv_cross::CompilerGLSL {m_code};
			const auto shader_stage_and_entrypoint = compiler.get_entry_points_and_stages();

			return shader_stage_and_entrypoint[0].name;
		}

		auto spir_v::code() const -> const spir_v_bytecode_t&
		{
			return m_code;
		}

		auto spir_v::stage() const -> const vk::ShaderStageFlagBits&
		{
			return m_stage;
		}

		auto spir_v::entrypoint() const -> const string::string_t
		{
			return m_entrypoint;
		}

		spir_v::operator const spir_v_bytecode_t&()
		{
			return m_code;
		}

		auto spir_v::glsl_to_spirv::translate_shader_stage(const vk::ShaderStageFlagBits& shader_stage) -> uint32_t
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

		auto spir_v::glsl_to_spirv::translate_shader(const vk::ShaderStageFlagBits& shader_stage,
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
	}
}
