module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <ranges>
#endif

module pipeline_resource_generator;

namespace
{
	auto generate_descriptor_set_bindings(const std::vector<lh::vulkan::shader_input>& shader_inputs)
	{
		auto bindings = std::vector<lh::vulkan::descriptor_set_layout::binding> {};

		for (const auto& shader_input : shader_inputs)
			bindings.emplace_back(shader_input.m_descriptor_binding,
								  shader_input.m_type,
								  shader_input.m_array_dimension);

		return bindings;
	}

	auto generate_unique_pipeline_inputs(
		const std::vector<std::pair<vk::ShaderStageFlagBits, lh::vulkan::shader_input>>& pipeline_inputs)
	{
		auto unique_pipeline_inputs = lh::vulkan::unique_pipeline_inputs {};

		for (const auto& [stage, input] : pipeline_inputs)
		{
			if (input.m_type == vk::DescriptorType::eUniformBuffer and
				not std::ranges::contains(unique_pipeline_inputs.m_uniform_buffer_descriptors, input))
				unique_pipeline_inputs.m_uniform_buffer_descriptors.push_back(input);

			if (input.m_type == vk::DescriptorType::eCombinedImageSampler and
				not std::ranges::contains(unique_pipeline_inputs.m_combined_image_sampler_descriptors, input))
				unique_pipeline_inputs.m_combined_image_sampler_descriptors.push_back(input);
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
																 const create_info& create_info)
			: m_spir_v {},
			  m_vertex_input_description {},
			  m_descriptor_set_layouts {},
			  m_pipeline_layout {nullptr},
			  m_shader_objects {},
			  m_uniform_buffers {},
			  m_uniform_buffer_subdata {},
			  m_resource_descriptor_buffer {physical_device, logical_device, memory_allocator}
		{
			auto pipeline_shader_inputs = std::vector<std::pair<vk::ShaderStageFlagBits, shader_input>> {};

			for (const auto& shader_path : shader_paths)
			{
				m_spir_v.emplace_back(shader_path);
				const auto& compiled_spir_v = m_spir_v.back();

				const auto shader_inputs = compiled_spir_v.reflect_shader_input();

				if (compiled_spir_v.stage() == vk::ShaderStageFlagBits::eVertex)
					m_vertex_input_description = generate_vertex_input_description(shader_inputs);

				for (const auto& shader_input : shader_inputs)
				{
					pipeline_shader_inputs.push_back({compiled_spir_v.stage(), shader_input});
				}
			}

			const auto pipeline_descriptor_sets = generate_descriptor_set_layouts(logical_device,
																				  pipeline_shader_inputs);

			auto descriptor_sets = std::vector<vk::DescriptorSetLayout> {};

			for (auto i = std::uint16_t {0}; const auto& stage_descriptor_sets : pipeline_descriptor_sets)
			{
				m_shader_objects.emplace_back(logical_device, m_spir_v[i].stage(), stage_descriptor_sets);

				for (const auto& stage_descriptor_set : stage_descriptor_sets)
					descriptor_sets.push_back(*stage_descriptor_set);

				i++;
			}
			m_pipeline_layout = {*logical_device, {{}, descriptor_sets}};

			const auto unique_pipeline_inputs = generate_unique_pipeline_inputs(pipeline_shader_inputs);

			const auto uniform_buffers_size =
				std::ranges::fold_left(unique_pipeline_inputs.m_uniform_buffer_descriptors,
									   vk::DeviceSize {},
									   [](auto size, const auto& element) {
										   size += element.m_size;
										   return std::move(size);
									   });

			m_uniform_buffers = {
				logical_device,
				memory_allocator,
				uniform_buffers_size,
				vulkan::mapped_buffer::create_info {.m_usage = vk::BufferUsageFlagBits::eUniformBuffer |
															   vk::BufferUsageFlagBits::eShaderDeviceAddress,
													.m_allocation_flags = vma::AllocationCreateFlagBits::eMapped}};

			m_uniform_buffer_subdata.m_buffer = &m_uniform_buffers;

			for (auto buffer_offset = vk::DeviceSize {};
				 const auto& uniform_buffer : unique_pipeline_inputs.m_uniform_buffer_descriptors)
			{
				m_uniform_buffer_subdata.m_subdata.emplace_back(buffer_offset, uniform_buffer.m_size);
				buffer_offset += uniform_buffer.m_size;
			}
		}

		auto pipeline_resource_generator::vertex_input_description() const -> const vulkan::vertex_input_description&
		{
			return m_vertex_input_description;
		}
		/*
		auto pipeline_resource_generator::descriptor_set_layouts() const
			-> const std::vector<vulkan::raii::descriptor_set_layout>&
		{
			return m_descriptor_set_layouts;
		}*/

		auto pipeline_resource_generator::pipeline_layout() const -> const vk::raii::PipelineLayout&
		{
			return m_pipeline_layout;
		}

		auto pipeline_resource_generator::shader_objects() const -> const std::vector<shader_object>&
		{
			return m_shader_objects;
		}

		auto pipeline_resource_generator::uniform_buffers() const -> const mapped_buffer&
		{
			return m_uniform_buffers;
		}

		auto pipeline_resource_generator::uniform_buffer_subdata() const -> const buffer_subdata&
		{
			return m_uniform_buffer_subdata;
		}

		auto pipeline_resource_generator::descriptor_buffer() -> vulkan::descriptor_buffer&
		{
			return m_resource_descriptor_buffer;
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
					offset = vertex_input.m_size;
				}
			vertex_bindings = {0, vertex_description_size, vk::VertexInputRate::eVertex, 1};

			return {vertex_bindings, vertex_attributes};
		}

		auto pipeline_resource_generator::generate_descriptor_set_layouts(
			const logical_device& logical_device,
			const std::vector<std::pair<vk::ShaderStageFlagBits, shader_input>>& pipeline_shader_inputs) const
			-> const std::vector<std::vector<vk::raii::DescriptorSetLayout>>
		{
			auto bindings =
				std::vector<std::pair<decltype(shader_input::m_descriptor_set), vk::DescriptorSetLayoutBinding>> {};
			auto descriptor_layouts = std::vector<std::vector<vk::raii::DescriptorSetLayout>> {};
			auto num_descriptor_sets = decltype(shader_input::m_descriptor_set) {};
			auto stages = std::vector<vk::ShaderStageFlagBits> {};

			// accumulate bindings from the entire pipeline
			for (const auto& shader_input : pipeline_shader_inputs)
			{
				// skip stage inputs
				if (shader_input.second.m_type == shader_input::s_stage_input_flag)
					continue;

				const auto& input = shader_input.second;

				bindings.push_back(
					{input.m_descriptor_set,
					 {input.m_descriptor_binding, input.m_type, input.m_array_dimension, shader_input.first}});

				// record the total number of descriptor sets
				num_descriptor_sets = std::max(num_descriptor_sets, input.m_descriptor_set);

				// record different pipeline stages
				if (not std::ranges::contains(stages, shader_input.first))
					stages.push_back(shader_input.first);
			};

			// check for same bindings from multiple stages and connect them
			for (auto& [set, binding] : bindings)
			{
				const auto& same_binding = std::ranges::find_if(bindings, [&binding](const auto& x) {
					return binding != x.second and (binding.binding == x.second.binding and
													binding.descriptorCount == x.second.descriptorCount and
													binding.descriptorType == x.second.descriptorType and
													binding.stageFlags != x.second.stageFlags);
				});

				binding.stageFlags |= same_binding->second.stageFlags;

				bindings.erase(std::remove(bindings.begin(), bindings.end(), same_binding));
			}
			/*
			// fill descriptor set layouts
			for (auto i = decltype(num_descriptor_sets) {}; i < num_descriptor_sets; i++)
			{
				auto set_bindings = std::vector<vk::DescriptorSetLayoutBinding> {};

				for (const auto& [set, binding] : bindings)
					if (set == i)
						set_bindings.push_back(binding);

				descriptor_layouts.emplace_back(
					*logical_device,
					vk::DescriptorSetLayoutCreateInfo {vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT,
													   set_bindings});
			}*/

			for (const auto& stage : stages)
			{
				auto stage_descriptor_sets = std::vector<vk::raii::DescriptorSetLayout> {};

				for (auto i = decltype(num_descriptor_sets) {}; i < num_descriptor_sets; i++)
				{
					auto set_bindings = std::vector<vk::DescriptorSetLayoutBinding> {};

					for (const auto& [set, binding] : bindings)
					{
						if (binding.stageFlags & stage and set == i)
							set_bindings.push_back(binding);
					}

					stage_descriptor_sets.emplace_back(
						*logical_device,
						vk::DescriptorSetLayoutCreateInfo {vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT,
														   set_bindings});
					i++;
				}

				descriptor_layouts.push_back(stage_descriptor_sets);
			}

			return descriptor_layouts;
		}
	}
}
