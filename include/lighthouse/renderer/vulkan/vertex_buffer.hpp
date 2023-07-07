#pragma once

#include "lighthouse/renderer/vulkan/raii_wrapper.hpp"
#include "lighthouse/renderer/vulkan/index_format.hpp"

#include <vector>

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;
		class memory_allocator;
		class buffer;
		class mapped_buffer;
		struct buffer_subdata;
		struct vertex_input_description;
		struct vertex;

		class vertex_buffer
		{
		public:
			struct create_info
			{};

			vertex_buffer(const logical_device&,
						  const memory_allocator&,
						  const std::vector<vertex>&,
						  const std::vector<vertex_index_t>&,
						  const vertex_input_description&,
						  const create_info& = {});

			auto vertex_input_description() const -> const vertex_input_description&;
			auto vertices() const -> const buffer_subdata;
			auto indices() const -> const buffer_subdata;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			std::unique_ptr<vulkan::vertex_input_description> m_vertex_input_description;
			std::unique_ptr<mapped_buffer> m_vertex_and_index_buffer;
			std::unique_ptr<buffer_subdata> m_vertex_and_index_suballocations;
		};
	}
}
