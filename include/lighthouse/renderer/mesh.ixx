module;

#if INTELLISENSE
#include <memory>
#include <vector>
#endif

export module mesh;

import node;
import geometry;
import lighthouse_utility;
import index_format;
import logical_device;
import memory_allocator;
import vertex_buffer;
import vertex_format;
import object_index;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	class mesh : public object_index<mesh>
	{
	public:
		struct create_info
		{};

		mesh();
		mesh(const vulkan::logical_device&,
			 const vulkan::memory_allocator&,
			 const std::vector<vulkan::vertex>&,
			 const std::vector<vulkan::vertex_index_t>&,
			 const geometry::aabb&,
			 non_owning_ptr<node> = nullptr);

		mesh(const mesh&) = delete;
		mesh& operator=(const mesh&) = delete;
		mesh(mesh&&) noexcept;
		mesh(const mesh&&) noexcept;
		mesh& operator=(mesh&&) noexcept;

		auto node() const -> const node&;
		auto vertices() const -> const std::vector<vulkan::vertex>&;
		auto indices() const -> const std::vector<vulkan::vertex_index_t>&;
		auto vertex_buffer() const -> const vulkan::vertex_buffer&;
		auto bounding_box() const -> const geometry::aabb&;

	private:
		std::shared_ptr<lh::node> m_node;
		std::vector<vulkan::vertex> m_vertices;
		std::vector<vulkan::vertex_index_t> m_indices;
		vulkan::vertex_buffer m_vertex_buffer;
		geometry::aabb m_bounding_box;
	};
}