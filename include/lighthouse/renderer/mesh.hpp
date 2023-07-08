#pragma once

#include "lighthouse/utility.hpp"
#include "lighthouse/renderer/vulkan/index_format.hpp"

#include <vector>

namespace lh
{
	// forward declarations
	class node;
	namespace vulkan
	{
		class logical_device;
		class memory_allocator;
		class vertex_buffer;
		struct vertex;
	};

	class mesh
	{
	public:
		struct create_info
		{};

		mesh(const vulkan::logical_device&,
			 const vulkan::memory_allocator&,
			 const std::vector<vulkan::vertex>&,
			 const std::vector<vulkan::vertex_index_t>&,
			 non_owning_ptr<node> = nullptr);

	private:
		std::shared_ptr<node> m_node;
		std::vector<vulkan::vertex> m_vertices;
		std::vector<vulkan::vertex_index_t> m_indices;
		std::unique_ptr<vulkan::vertex_buffer> m_vertex_buffer;
	};
}
