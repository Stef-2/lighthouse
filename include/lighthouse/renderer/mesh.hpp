#pragma once

#include "lighthouse/utility.hpp"

#include <vector>

namespace lh
{
	// forward declarations
	class node;
	struct vertex;
	namespace vulkan
	{
		class vertex_buffer
	};

	class mesh
	{
	public:
		using index_buffer_t = uint32_t;

		struct create_info
		{};

		mesh(const node&, const std::vector<vertex>&, const std::vector<index_buffer_t>&);

	private:
		non_owning_ptr<node> m_node;
		std::unique_ptr<vertex_buffer> m_vertex_buffer;
		std::vector<index_buffer_t> m_index_buffer;
	};
}
