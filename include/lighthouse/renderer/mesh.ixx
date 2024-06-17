module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <memory>
#include <vector>
#endif

export module mesh;

import node;
import geometry;
import buffer;
import lighthouse_utility;
import object_index;
import registry;

#if not INTELLISENSE
import std.core;
#endif


export namespace lh
{
	class mesh : public object_index<mesh>, public registry_entry<mesh>
	{
	public:
		struct create_info
		{};

		mesh();
		mesh(const vulkan::buffer_subdata<vulkan::buffer>&, const geometry::aabb&, non_owning_ptr<node> = nullptr);

		mesh(const mesh&) = delete;
		mesh& operator=(const mesh&) = delete;
		mesh(mesh&&) noexcept;
		mesh(const mesh&&) noexcept;
		mesh& operator=(mesh&&) noexcept;

		auto node() const -> const node&;
		auto vertex_subdata() const -> const vulkan::buffer_subdata<vulkan::buffer>::subdata&;
		auto index_subdata() const -> const vulkan::buffer_subdata<vulkan::buffer>::subdata&;
		auto bounding_box() const -> const geometry::aabb&;
		auto index_count() const -> const std::size_t;

		auto bind(const vk::raii::CommandBuffer&) const -> void;

	private:
		std::shared_ptr<lh::node> m_node;
		vulkan::buffer_subdata<vulkan::buffer> m_vertex_and_index_subdata;
		geometry::aabb m_bounding_box;
		std::size_t m_index_count;
	};
}