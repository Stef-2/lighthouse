module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <memory>
#include <vector>
#endif

export module mesh;

import node;
import geometry;
import buffer;
import lighthouse_utility;
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
		mesh(const vulkan::buffer_subdata<vulkan::buffer>&, const geometry::aabb&, non_owning_ptr<node> = nullptr);

		mesh(const mesh&) = delete;
		mesh& operator=(const mesh&) = delete;
		mesh(mesh&&) noexcept;
		mesh(const mesh&&) noexcept;
		mesh& operator=(mesh&&) noexcept;

		auto node() const -> const node&;
		auto vertex_subdata() const -> const vulkan::buffer_subdata::subdata&;
		auto index_subdata() const -> const vulkan::buffer_subdata::subdata&;
		auto bounding_box() const -> const geometry::aabb&;
		auto device_size() const -> const vk::DeviceSize;

	private:
		std::shared_ptr<lh::node> m_node;
		vulkan::buffer_subdata<vulkan::buffer> m_vertex_and_index_subdata;
		geometry::aabb m_bounding_box;
	};
}