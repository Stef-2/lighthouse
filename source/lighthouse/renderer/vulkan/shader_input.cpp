#include "lighthouse/renderer/vulkan/shader_input.hpp"
#include "lighthouse/string/string.hpp"

#include <functional>

auto lh::vulkan::shader_input::hash() const -> const std::size_t
{
	auto combined_input = string::string_t {
		std::to_string(m_descriptor_set) + std::to_string(m_descriptor_layout) + std::to_string(m_descriptor_binding) +
		std::to_string(std::to_underlying(m_type)) + std::to_string(std::to_underlying(m_data_type)) +
		std::to_string(m_rows) + std::to_string(m_columns) + std::to_string(m_array_dimension) +
		std::to_string(m_size)};

	for (const auto& member : m_members)
		combined_input += std::to_string(std::to_underlying(member.m_data_type)) + std::to_string(member.m_rows) +
						  std::to_string(member.m_colums) + std::to_string(member.m_array_dimension) +
						  std::to_string(member.m_size) + std::to_string(member.m_offset);

	return std::hash<string::string_t> {}(combined_input);
}
