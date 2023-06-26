#pragma once

#include "lighthouse/utility.hpp"
#include "lighthouse/renderer/index_format.hpp"

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
		struct create_info
		{};

		mesh(const node&, const std::vector<vertex>&, const std::vector<index_buffer_t>&);

	private:
		non_owning_ptr<node> m_node;
		std::unique_ptr<vulkan::vertex_buffer> m_vertex_buffer;
		std::vector<index_format_t> m_index_buffer;
	};
}
