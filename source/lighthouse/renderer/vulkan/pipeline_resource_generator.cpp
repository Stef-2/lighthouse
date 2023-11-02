module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <ranges>
#endif

module pipeline_resource_generator;

import input;

namespace
{
	auto generate_unique_pipeline_inputs(
		const std::vector<std::pair<vk::ShaderStageFlagBits, lh::vulkan::shader_input>>& pipeline_inputs)
	{
		auto unique_pipeline_inputs = lh::vulkan::unique_pipeline_inputs {};

		for (const auto& [stage, input] : pipeline_inputs)
		{
			if (input.m_type == vk::DescriptorType::eUniformBuffer and
				not std::ranges::contains(unique_pipeline_inputs.m_uniform_buffer_descriptors, input))
				unique_pipeline_inputs.m_uniform_buffer_descriptors.push_back(input);

			if (input.m_type == vk::DescriptorType::eStorageBuffer and
				not std::ranges::contains(unique_pipeline_inputs.m_storage_buffer_descriptors, input) and
				input.m_descriptor_set != 3)
				unique_pipeline_inputs.m_storage_buffer_descriptors.push_back(input);

			if (input.m_type == vk::DescriptorType::eCombinedImageSampler and
				not std::ranges::contains(unique_pipeline_inputs.m_combined_image_sampler_descriptors, input))
				unique_pipeline_inputs.m_combined_image_sampler_descriptors.push_back(input);

			if (input.m_type == vk::DescriptorType::eUniformBuffer and
				input.m_storage_class == lh::vulkan::shader_input::storage_class::push_constant)
				unique_pipeline_inputs.m_push_constant = input;
		}

		return unique_pipeline_inputs;
	}
}

namespace lh
{
	namespace vulkan
	{
		pipeline_resource_generator::pipeline_resource_generator(const physical_device& physical_device,
																 const logical_device& logical_device,
																 const memory_allocator& memory_allocator,
																 const pipeline_glsl_code& shader_paths,
																 const global_descriptor& global_descriptor,
																 const create_info& create_info)
			: m_vertex_input_description {}, m_shader_pipeline {}, m_resource_descriptor_buffer {}
		{
			auto pipeline_shader_inputs = std::vector<std::pair<vk::ShaderStageFlagBits, shader_input>> {};
			auto spir_v = std::vector<vulkan::spir_v> {};

			for (const auto& shader_path : shader_paths)
			{
				spir_v.emplace_back(lh::input::read_file(shader_path));
				const auto& compiled_spir_v = spir_v.back();

				const auto shader_inputs = compiled_spir_v.reflect_shader_input();

				if (compiled_spir_v.stage() == vk::ShaderStageFlagBits::eVertex)
					m_vertex_input_description = generate_vertex_input_description(shader_inputs);

				for (const auto& shader_input : shader_inputs)
					pipeline_shader_inputs.push_back({compiled_spir_v.stage(), shader_input});
			}

			const auto unique_pipeline_inputs = generate_unique_pipeline_inputs(pipeline_shader_inputs);

			auto pipeline_data = std::vector<shader_pipeline::individual_stage_data_t> {};
			for (const auto& stage_spir_v : spir_v)
				pipeline_data.emplace_back(stage_spir_v, global_descriptor.descriptor_set_layouts());

			m_shader_pipeline = {logical_device, pipeline_data};

			auto resource_buffer_subdata =
				std::vector<descriptor_resource_buffer::create_info::binding_type_and_subdata_t> {};

			auto resoruce_descriptor_buffer_usage = vk::BufferUsageFlags {
				vk::BufferUsageFlagBits::eShaderDeviceAddress};

			auto buffer_offset = vk::DeviceSize {};

			for (const auto& uniform_buffer : unique_pipeline_inputs.m_uniform_buffer_descriptors)
			{
				resource_buffer_subdata.emplace_back(
					std::pair {vk::DescriptorType::eUniformBuffer,
							   buffer_subdata::subdata {buffer_offset, uniform_buffer.m_size}});
				resoruce_descriptor_buffer_usage |= vk::BufferUsageFlagBits::eUniformBuffer;

				buffer_offset += uniform_buffer.m_size;
			}

			for (const auto& storage_buffer : unique_pipeline_inputs.m_storage_buffer_descriptors)
			{
				resource_buffer_subdata.emplace_back(
					std::pair {vk::DescriptorType::eStorageBuffer,
							   buffer_subdata::subdata {buffer_offset, storage_buffer.m_size}});
				resoruce_descriptor_buffer_usage |= vk::BufferUsageFlagBits::eStorageBuffer;

				buffer_offset += storage_buffer.m_size;
			}

			m_resource_descriptor_buffer = {physical_device,
											logical_device,
											memory_allocator,
											buffer_offset,
											{{.m_usage = resoruce_descriptor_buffer_usage}, resource_buffer_subdata}};
		}

		auto pipeline_resource_generator::vertex_input_description() const -> const vulkan::vertex_input_description&
		{
			return m_vertex_input_description;
		}

		auto pipeline_resource_generator::shader_pipeline() const -> const vulkan::shader_pipeline&
		{
			return m_shader_pipeline;
		}

		auto pipeline_resource_generator::descriptor_buffer() const -> const vulkan::descriptor_resource_buffer&
		{
			return m_resource_descriptor_buffer;
		}

		auto pipeline_resource_generator::bind(const vk::raii::CommandBuffer& command_buffer) const -> void
		{
			command_buffer.setVertexInputEXT(m_vertex_input_description.m_bindings,
											 m_vertex_input_description.m_attributes);

			m_shader_pipeline.bind(command_buffer);
		}

		auto pipeline_resource_generator::translate_shader_input_format(const shader_input& shader_input) const
			-> const vk::Format
		{
			auto format = vk::Format {};

			switch (shader_input.m_data_type)
			{
				case shader_input::data_type::boolean: break;

				case shader_input::data_type::integer_16:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR16Sint; break;
						case 2: format = vk::Format::eR16G16Sint; break;
						case 3: format = vk::Format::eR16G16B16Sint; break;
						case 4: format = vk::Format::eR16G16B16A16Sint; break;
						default: break;
					}
					break;
				case shader_input::data_type::unsigned_integer_16:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR16Uint; break;
						case 2: format = vk::Format::eR16G16Uint; break;
						case 3: format = vk::Format::eR16G16B16Uint; break;
						case 4: format = vk::Format::eR16G16B16A16Uint; break;
						default: break;
					}
					break;
				case shader_input::data_type::integer_32:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR32Sint; break;
						case 2: format = vk::Format::eR32G32Sint; break;
						case 3: format = vk::Format::eR32G32B32Sint; break;
						case 4: format = vk::Format::eR32G32B32A32Sint; break;
						default: break;
					}
					break;
				case shader_input::data_type::unsigned_integer_32:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR32Uint; break;
						case 2: format = vk::Format::eR32G32Uint; break;
						case 3: format = vk::Format::eR32G32B32Uint; break;
						case 4: format = vk::Format::eR32G32B32A32Uint; break;
						default: break;
					}
					break;
				case shader_input::data_type::integer_64:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR64Sint; break;
						case 2: format = vk::Format::eR64G64Sint; break;
						case 3: format = vk::Format::eR64G64B64Sint; break;
						case 4: format = vk::Format::eR64G64B64A64Sint; break;
						default: break;
					}
					break;
				case shader_input::data_type::unsigned_integer_64:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR64Uint; break;
						case 2: format = vk::Format::eR64G64Uint; break;
						case 3: format = vk::Format::eR64G64B64Uint; break;
						case 4: format = vk::Format::eR64G64B64A64Uint; break;
						default: break;
					}
					break;

				case shader_input::data_type::float_16:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR16Sfloat; break;
						case 2: format = vk::Format::eR16G16Sfloat; break;
						case 3: format = vk::Format::eR16G16B16Sfloat; break;
						case 4: format = vk::Format::eR16G16B16A16Sfloat; break;
						default: break;
					}
					break;
				case shader_input::data_type::float_32:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR32Sfloat; break;
						case 2: format = vk::Format::eR32G32Sfloat; break;
						case 3: format = vk::Format::eR32G32B32Sfloat; break;
						case 4: format = vk::Format::eR32G32B32A32Sfloat; break;
						default: break;
					}
					break;
				case shader_input::data_type::float_64:
					switch (shader_input.m_rows)
					{
						case 1: format = vk::Format::eR64Sfloat; break;
						case 2: format = vk::Format::eR64G64Sfloat; break;
						case 3: format = vk::Format::eR64G64B64Sfloat; break;
						case 4: format = vk::Format::eR64G64B64A64Sfloat; break;
						default: break;
					}
					break;

				case shader_input::data_type::structure: break;
				case shader_input::data_type::image: break;
				case shader_input::data_type::sampled_image: break;
				case shader_input::data_type::sampler: break;
				default: break;
			}

			return format;
		}
		auto pipeline_resource_generator::generate_vertex_input_description(
			const std::vector<shader_input>& shader_inputs) -> const vulkan::vertex_input_description
		{
			auto vertex_bindings = vk::VertexInputBindingDescription2EXT {};
			auto vertex_attributes = std::vector<vk::VertexInputAttributeDescription2EXT> {};

			auto vertex_description_size = std::uint32_t {};
			auto offset = std::uint32_t {};

			for (const auto& vertex_input : shader_inputs)
				if (vertex_input.m_type == shader_input::s_stage_input_flag)
				{
					vertex_description_size += vertex_input.m_size;

					vertex_attributes.emplace_back(vertex_input.m_descriptor_location,
												   vertex_input.m_descriptor_binding,
												   translate_shader_input_format(vertex_input),
												   offset);
					offset += vertex_input.m_size;
				}
			vertex_bindings = {0, vertex_description_size, vk::VertexInputRate::eVertex, 1};

			return {vertex_bindings, vertex_attributes};
		}

	}
}
