module;

module shader_input;

namespace lh
{
	namespace vulkan
	{
		auto shader_input::operator()(const shader_input& shader_input) const -> std::size_t
		{
			auto combined_input = string::string_t {
				std::to_string(shader_input.m_descriptor_set) + std::to_string(shader_input.m_descriptor_location) +
				std::to_string(shader_input.m_descriptor_binding) +
				std::to_string(std::to_underlying(shader_input.m_type)) +
				std::to_string(std::to_underlying(shader_input.m_data_type)) + std::to_string(shader_input.m_rows) +
				std::to_string(shader_input.m_columns) + std::to_string(shader_input.m_array_dimension) +
				std::to_string(shader_input.m_size)};

			for (const auto& member : shader_input.m_members)
				combined_input += std::to_string(std::to_underlying(member.m_data_type)) +
								  std::to_string(member.m_rows) + std::to_string(member.m_colums) +
								  std::to_string(member.m_array_dimension) + std::to_string(member.m_size) +
								  std::to_string(member.m_offset);

			return std::hash<string::string_t> {}(combined_input);
		}

		auto shader_input::operator==(const shader_input& shader_input) const -> bool
		{
			return (*this)(shader_input) == shader_input(*this);
		}
	}
}
