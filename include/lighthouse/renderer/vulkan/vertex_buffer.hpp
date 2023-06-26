#pragma once

#include "lighthouse/renderer/vulkan/raii_wrapper.hpp"
#include "lighthouse/renderer/vulkan/buffer.hpp"

#include <vector>

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;
		class memory_allocator;
		class shader_object;

		class vertex_buffer
		{
		public:
			struct vertex_input_description
			{
				vk::VertexInputBindingDescription2EXT m_bindings;
				std::vector<vk::VertexInputAttributeDescription2EXT> m_attributes;
			};

			struct create_info
			{};

			vertex_buffer(const physical_device&,
						  const logical_device&,
						  const memory_allocator&,
						  const vertex_input_description&,
						  const create_info& = {});

			auto vertex_input() const -> const vertex_input_description&;
			auto vertices() const -> const mapped_buffer&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			vertex_input_description m_vertex_input;
			mapped_buffer m_vertices;
		};
	}
}
