module;

module material;

namespace lh
{
	material::material(const vulkan::physical_device& physical_device,
					   const vulkan::logical_device& logical_device,
					   const vulkan::memory_allocator& memory_allocator,
					   vulkan::queue& queue,
					   const std::vector<std::filesystem::path>& texture_paths,
					   vulkan::descriptor_buffer& descriptor_buffer,
					   const create_info& create_info)
		: m_textures {}
	{
		m_textures.reserve(texture_paths.size());

		for (const auto& path : texture_paths)
		{
			m_textures.emplace_back(physical_device,
									logical_device,
									memory_allocator,
									queue,
									std::vector<std::filesystem::path> {path},
									descriptor_buffer);
		}
	}

	auto material::textures() const -> const std::vector<vulkan::texture>&
	{
		return m_textures;
	}
}
