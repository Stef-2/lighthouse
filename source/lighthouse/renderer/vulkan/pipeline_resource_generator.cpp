#include "lighthouse/renderer/vulkan/pipeline_resource_generator.hpp"
#include "lighthouse/renderer/vulkan/physical_device.hpp"
#include "lighthouse/renderer/vulkan/logical_device.hpp"
#include "lighthouse/renderer/vulkan/memory_allocator.hpp"
#include "lighthouse/renderer/vulkan/spir_v.hpp"
#include "lighthouse/renderer/vulkan/buffer.hpp"
#include "lighthouse/renderer/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/renderer/vulkan/shader_object.hpp"
#include "lighthouse/renderer/vulkan/descriptor_buffer.hpp"
#include "lighthouse/renderer/vulkan/vertex_input_description.hpp"

lh::vulkan::pipeline_resource_generator::pipeline_resource_generator(const physical_device& physical_device,
																	 const logical_device& logical_device,
																	 const memory_allocator& memory_allocator,
																	 const pipeline_spir_v_code spir_v_code,
																	 const create_info& create_info)
{}

auto lh::vulkan::pipeline_resource_generator::shader_input_hash(const shader_input& shader_input) const
	-> const std::size_t
{
	auto combined_input = string::string_t {
		std::to_string(shader_input.m_descriptor_set) + std::to_string(shader_input.m_descriptor_location) +
		std::to_string(shader_input.m_descriptor_binding) + std::to_string(std::to_underlying(shader_input.m_type)) +
		std::to_string(std::to_underlying(shader_input.m_data_type)) + std::to_string(shader_input.m_rows) +
		std::to_string(shader_input.m_columns) + std::to_string(shader_input.m_array_dimension) +
		std::to_string(shader_input.m_size)};

	for (const auto& member : shader_input.m_members)
		combined_input += std::to_string(std::to_underlying(member.m_data_type)) + std::to_string(member.m_rows) +
						  std::to_string(member.m_colums) + std::to_string(member.m_array_dimension) +
						  std::to_string(member.m_size) + std::to_string(member.m_offset);

	return std::hash<string::string_t> {}(combined_input);
}

auto lh::vulkan::pipeline_resource_generator::translate_shader_input_format(const shader_input& shader_input) const
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
auto lh::vulkan::pipeline_resource_generator::vertex_input_description(const std::vector<shader_input>& shader_inputs)
	-> const vulkan::vertex_input_description
{
	constexpr auto byte_divisor = uint8_t {8};

	auto vertex_bindings = vk::VertexInputBindingDescription2EXT {};
	auto vertex_attributes = std::vector<vk::VertexInputAttributeDescription2EXT> {};

	auto vertex_description_size = std::uint32_t {};
	auto offset = std::uint32_t {};

	for (const auto& vertex_input : shader_inputs)
		if (vertex_input.m_type == shader_input::input_type::stage_input)
		{
			vertex_description_size += vertex_input.m_size * vertex_input.m_rows / byte_divisor;

			vertex_attributes.emplace_back(vertex_input.m_descriptor_location,
										   vertex_input.m_descriptor_binding,
										   vertex_input.translate_format(),
										   offset);
			offset = vertex_input.m_size * vertex_input.m_rows / byte_divisor;
		}
	vertex_bindings = {0, vertex_description_size, vk::VertexInputRate::eVertex, 1};

	return {vertex_bindings, vertex_attributes};
}

auto lh::vulkan::pipeline_resource_generator::descriptor_set_layout(const shader_input&)
	-> const vk::raii::DescriptorSetLayout
{}
