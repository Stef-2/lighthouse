module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#include <optional>
#endif

export module vertex_buffer;

import buffer;
import raii_wrapper;
import logical_device;
import memory_allocator;
import vertex_input_description;
import vertex_format;
import index_format;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		class vertex_buffer
		{
		public:
			struct create_info
			{};

			vertex_buffer();
			vertex_buffer(const logical_device&,
						  const memory_allocator&,
						  const std::vector<vertex>&,
						  const std::vector<vertex_index_t>&,
						  const std::optional<buffer_subdata>& = {},
						  const create_info& = {});

			vertex_buffer(const vertex_buffer&) = delete;
			vertex_buffer& operator=(const vertex_buffer&) = delete;
			vertex_buffer(vertex_buffer&&) noexcept;
			vertex_buffer& operator=(vertex_buffer&&) noexcept;

			auto vertices() const -> const buffer_subdata;
			auto indices() const -> const buffer_subdata;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			std::optional<mapped_buffer> m_vertex_and_index_buffer;
			buffer_subdata m_vertex_and_index_suballocations;
		};
	}
}