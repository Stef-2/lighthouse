module;

#if INTELLISENSE
	#include "vulkan/vulkan_raii.hpp"

	#include <iostream>
#endif

#include "vulkan/shaderc/shaderc.hpp"
#include "vulkan/spirv_cross/spirv_reflect.hpp"

module spir_v;

import data_type;
import file_system;
import input;
import output;

namespace
{
	struct includer : public shaderc::CompileOptions::IncluderInterface
	{
		struct user_data
		{
			lh::string::string_t m_content;
			lh::string::string_t m_path;
		};

		auto GetInclude(const char* requested_source,
						shaderc_include_type include_type,
						const char* requesting_source,
						std::size_t include_depth) -> shaderc_include_result* override
		{
			auto data = new shaderc_include_result {};

			auto path = lh::file_system::path(lh::file_system::directory::shaders);

			if (include_type == shaderc_include_type::shaderc_include_type_relative)
			{
				if (not std::strstr(requested_source, "include"))
					path.append("include").append(requested_source);
				else
					path.append(requested_source);
			} else
				path = lh::filepath_t {requested_source};

			if (not std::filesystem::exists(path))
				lh::output::warning() << "failed to include shader file: " << requested_source;

			data->user_data = new user_data {lh::input::read_text_file(path), path.string()};

			data->content = static_cast<user_data*>(data->user_data)->m_content.c_str();
			data->content_length = static_cast<user_data*>(data->user_data)->m_content.length();
			data->source_name = static_cast<user_data*>(data->user_data)->m_path.c_str();
			data->source_name_length = static_cast<user_data*>(data->user_data)->m_path.length();

			return data;
		}

		auto ReleaseInclude(shaderc_include_result* data) -> void override
		{
			delete static_cast<user_data*>(data->user_data);
			delete data;
		}
	};

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

			default:
				lh::output::warning() << "unrecognized spir_v base type: " + spirv_type;
				return return_type::invalid;
		}
	}

	auto translate_storage_class(const spv::StorageClass& storage_class)
	{
		using return_type = lh::vulkan::shader_input::storage_class;

		switch (storage_class)
		{
			case spv::StorageClass::StorageClassUniformConstant: return return_type::uniform_constant;
			case spv::StorageClass::StorageClassInput: return return_type::input;
			case spv::StorageClass::StorageClassUniform: return return_type::uniform;
			case spv::StorageClass::StorageClassOutput: return return_type::output;
			case spv::StorageClass::StorageClassWorkgroup: return return_type::workgroup;
			case spv::StorageClass::StorageClassCrossWorkgroup: return return_type::cross_workgroup;
			case spv::StorageClass::StorageClassPrivate: return return_type::private_storage;
			case spv::StorageClass::StorageClassFunction: return return_type::function;
			case spv::StorageClass::StorageClassGeneric: return return_type::generic;
			case spv::StorageClass::StorageClassPushConstant: return return_type::push_constant;
			case spv::StorageClass::StorageClassAtomicCounter: return return_type::atomic_counter;
			case spv::StorageClass::StorageClassImage: return return_type::image;
			case spv::StorageClass::StorageClassStorageBuffer: return return_type::storage_buffer;
			case spv::StorageClass::StorageClassCallableDataKHR: return return_type::callable_data_KHR;
			case spv::StorageClass::StorageClassIncomingCallableDataKHR: return return_type::incoming_callable_data_KHR;
			case spv::StorageClass::StorageClassRayPayloadKHR: return return_type::ray_payload_KHR;
			case spv::StorageClass::StorageClassHitAttributeKHR: return return_type::hit_attribute_KHR;
			case spv::StorageClass::StorageClassIncomingRayPayloadKHR: return return_type::incoming_ray_payload_KHR;
			case spv::StorageClass::StorageClassShaderRecordBufferKHR: return return_type::shader_record_buffer_KHR;
			case spv::StorageClass::StorageClassPhysicalStorageBuffer: return return_type::physical_storage_buffer;
			case spv::StorageClass::StorageClassTaskPayloadWorkgroupEXT: return return_type::task_payload_workgroup_EXT;

			default:
				lh::output::warning() << "unrecognized spir_v storage class: " + storage_class;
				return return_type::invalid;
		}
	}

	auto create_shader_input(const spirv_cross::CompilerGLSL& compiler,
							 const spirv_cross::Resource& resource,
							 const vk::DescriptorType& input_type)

	{
		constexpr auto byte_divisor = std::uint8_t {8};
		constexpr auto spir_v_word_length = std::uint8_t {4};

		const auto set = compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet);
		const auto binding = compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding);
		const auto location = compiler.get_decoration(resource.id, spv::Decoration::DecorationLocation);
		const auto storage_class = compiler.get_storage_class(resource.id);
		const auto data_type = compiler.get_type_from_variable(resource.id).basetype;
		const auto rows = compiler.get_type_from_variable(resource.id).vecsize;
		const auto columns = compiler.get_type_from_variable(resource.id).columns;
		const auto array_dimension = compiler.get_type(resource.type_id).array.empty()
										 ? 1
										 : compiler.get_type(resource.type_id).array[0];
		const auto size = compiler.get_type(resource.base_type_id).member_types.empty()
							  ? compiler.get_type_from_variable(resource.id).width / byte_divisor * rows * columns
							  : compiler.get_declared_struct_size(compiler.get_type(resource.base_type_id));

		auto set_offset = std::uint32_t {};
		compiler.get_binary_offset_for_decoration(resource.id, spv::Decoration::DecorationDescriptorSet, set_offset);
		auto binding_offset = std::uint32_t {};
		compiler.get_binary_offset_for_decoration(resource.id, spv::Decoration::DecorationBinding, binding_offset);
		auto location_offset = std::uint32_t {};
		compiler.get_binary_offset_for_decoration(resource.id, spv::Decoration::DecorationLocation, location_offset);

		auto input = lh::vulkan::shader_input {set,
											   location,
											   binding,
											   input_type,
											   translate_storage_class(storage_class),
											   translate_data_type(data_type),
											   static_cast<std::uint8_t>(rows),
											   static_cast<std::uint8_t>(columns),
											   array_dimension,
											   size,
											   set_offset * spir_v_word_length,
											   binding_offset * spir_v_word_length,
											   location_offset * spir_v_word_length};

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
		// #pragma optimize("", on)
		spir_v::spir_v(const glsl_code_t& glsl_code, const create_info& create_info)
			: m_code {glsl_to_spirv::translate_shader(glsl_code)} //, m_entrypoint {}, m_stage {}
		{														  /*
																	 if (create_info.m_entrypoint and create_info.m_stage)
																	 {
																		 m_entrypoint = *create_info.m_entrypoint;
																		 m_stage = *create_info.m_stage;
																	 } else
																	 {
																		 const auto [entrypoint, stage] = reflect_shader_entrypoint_and_stage();
														
																		 m_entrypoint = entrypoint;
																		 m_stage = stage;
																	 }*/
		}

		spir_v::spir_v(const spir_v_code_t& spir_v_code, const create_info& create_info)
			: m_code {spir_v_code} //, m_entrypoint {}, m_stage {}
		{						   /*
									  if (create_info.m_entrypoint and create_info.m_stage)
									  {
										  m_entrypoint = *create_info.m_entrypoint;
										  m_stage = *create_info.m_stage;
									  } else
									  {
										  const auto [entrypoint, stage] = reflect_shader_entrypoint_and_stage();
						 
										  m_entrypoint = entrypoint;
										  m_stage = stage;
									  }*/
		}

		auto spir_v::reflect_shader_input() const -> std::vector<shader_input>
		{
			auto compiler = std::make_unique<spirv_cross::CompilerGLSL>(m_code);
			auto entrypoint_and_stage = compiler->get_entry_points_and_stages();
			auto resources = compiler->get_shader_resources();
			if constexpr (remove_inactive_inputs)
			{
				auto interface_variables = compiler->get_active_interface_variables();
				resources = compiler->get_shader_resources(interface_variables);

				compiler->set_enabled_interface_variables(std::move(interface_variables));
			}

			auto shader_inputs = std::vector<shader_input> {};
			shader_inputs.reserve(resources.stage_inputs.size() + resources.uniform_buffers.size() +
								  resources.storage_buffers.size() + resources.sampled_images.size() +
								  resources.push_constant_buffers.size());

			for (const auto& resource : resources.stage_inputs)
				shader_inputs.emplace_back(create_shader_input(*compiler, resource, shader_input::s_stage_input_flag));

			for (const auto& resource : resources.uniform_buffers)
				shader_inputs.emplace_back(
					create_shader_input(*compiler, resource, vk::DescriptorType::eUniformBuffer));

			for (const auto& resource : resources.storage_buffers)
				shader_inputs.emplace_back(
					create_shader_input(*compiler, resource, vk::DescriptorType::eStorageBuffer));

			for (const auto& resource : resources.sampled_images)
				shader_inputs.emplace_back(
					create_shader_input(*compiler, resource, vk::DescriptorType::eCombinedImageSampler));

			for (const auto& resource : resources.push_constant_buffers)
				shader_inputs.emplace_back(
					create_shader_input(*compiler, resource, vk::DescriptorType::eUniformBuffer));

			std::ranges::sort(shader_inputs, [](const auto& x, const auto& y) {
				switch (x.m_type)
				{
					case shader_input::s_stage_input_flag: return (x.m_descriptor_location < y.m_descriptor_location);
					case vk::DescriptorType::eUniformBuffer:
						return (x.m_descriptor_binding < y.m_descriptor_binding and
								x.m_descriptor_set == y.m_descriptor_set);
					case vk::DescriptorType::eStorageBuffer:
						return (x.m_descriptor_binding < y.m_descriptor_binding and
								x.m_descriptor_set == y.m_descriptor_set);
					case vk::DescriptorType::eCombinedImageSampler:
						return (x.m_descriptor_binding < y.m_descriptor_binding and
								x.m_descriptor_set == y.m_descriptor_set);
					default: return true;
				}
			});

			return shader_inputs;
		}

		auto spir_v::reflect_shader_entrypoint_and_stage() const
			-> const std::pair<string::string_t, vk::ShaderStageFlagBits>
		{
			const auto compiler = spirv_cross::CompilerGLSL {m_code};
			const auto shader_stage_and_entrypoint = compiler.get_entry_points_and_stages();
			auto shader_stage = vk::ShaderStageFlagBits {};

			switch (shader_stage_and_entrypoint[0].execution_model)
			{
				case spv::ExecutionModel::ExecutionModelVertex: shader_stage = vk::ShaderStageFlagBits::eVertex; break;
				case spv::ExecutionModel::ExecutionModelFragment:
					shader_stage = vk::ShaderStageFlagBits::eFragment;
					break;
				case spv::ExecutionModel::ExecutionModelGLCompute:
					shader_stage = vk::ShaderStageFlagBits::eCompute;
					break;
				default: shader_stage = vk::ShaderStageFlagBits::eAll;
			}

			return {shader_stage_and_entrypoint[0].name, shader_stage};
		}

		auto spir_v::code() const -> const spir_v_code_t&
		{
			return m_code;
		}
		auto spir_v::data_range() const -> const lh::data_range
		{
			return {m_code.data(), m_code.size() * sizeof spir_v_code_t::value_type};
		}
		/*
		auto spir_v::stage() const -> const vk::ShaderStageFlagBits
		{
			return m_stage;
		}

		auto spir_v::entrypoint() const -> const string::string_t&
		{
			return m_entrypoint;
		}*/

		auto spir_v::cache_binary_data(const filepath_t& path) const -> void {}

		auto spir_v::glsl_to_spirv::translate_shader(const glsl_code_t& shader_code) -> spir_v_code_t

		{
			const auto compiler = shaderc::Compiler {};
			auto compile_options = shaderc::CompileOptions {};
			compile_options.SetOptimizationLevel(shaderc_optimization_level_performance);

			compile_options.SetPreserveBindings(not remove_inactive_inputs);

			compile_options.SetIncluder(std::make_unique<includer>());

			const auto result = compiler.CompileGlslToSpv(shader_code.c_str(),
														  shaderc_glsl_infer_from_source,
														  "shader",
														  compile_options);

			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				auto error_message = result.GetErrorMessage();
				output::error() << error_message;
				std::cout << error_message;
				std::abort();
			}
			return {result.cbegin(), result.cend()};
		}
	}
}
