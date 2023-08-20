module;

export module mesh;

import node;
import bounding_volume;
import lh_utility;
import index_format;
import logical_device;
import memory_allocator;
import vertex_buffer;
import vertex_format;
import std.core;

export namespace lh
{
	class mesh
	{
	public:
		struct create_info
		{};

		mesh(const vulkan::logical_device&,
			 const vulkan::memory_allocator&,
			 const std::vector<vulkan::vertex>&,
			 const std::vector<vulkan::vertex_index_t>&,
			 const bounding_box&,
			 non_owning_ptr<node> = nullptr);

		auto node() const -> const node&;
		auto vertices() const -> const std::vector<vulkan::vertex>&;
		auto indices() const -> const std::vector<vulkan::vertex_index_t>&;
		auto vertex_buffer() const -> const vulkan::vertex_buffer&;

	private:
		std::shared_ptr<lh::node> m_node;
		std::vector<vulkan::vertex> m_vertices;
		std::vector<vulkan::vertex_index_t> m_indices;
		vulkan::vertex_buffer m_vertex_buffer;
		bounding_box m_bounding_box;
	};
}
