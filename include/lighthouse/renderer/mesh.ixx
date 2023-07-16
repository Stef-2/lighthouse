module;
#pragma once

#include <vector>

export module mesh;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/index_format.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/memory_allocator.ixx"
#include "lighthouse/renderer/vulkan/vertex_buffer.ixx"
#include "lighthouse/renderer/vulkan/vertex_format.ixx"
#include "lighthouse/renderer/bounding_volume.ixx"
#include "lighthouse/node.ixx"
#include "lighthouse/utility.ixx"
#else
import node;
import bounding_volume;
import utility;
import index_format;
import logical_device;
import memory_allocator;
import vertex_buffer;
import vertex_format;
#endif

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
		std::unique_ptr<vulkan::vertex_buffer> m_vertex_buffer;
		std::unique_ptr<bounding_box> m_bounding_box;
	};
}
