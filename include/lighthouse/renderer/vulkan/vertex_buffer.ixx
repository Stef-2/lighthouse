module;
#pragma once

export module vertex_buffer;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/memory_allocator.ixx"
#include "lighthouse/renderer/vulkan/buffer.ixx"
#include "lighthouse/renderer/vulkan/vertex_input_description.ixx"
#include "lighthouse/renderer/vulkan/vertex_format.ixx"
#include "lighthouse/renderer/vulkan/index_format.ixx"
#else
import raii_wrapper;
import logical_device;
import memory_allocator;
import buffer;
import vertex_input_description;
import vertex_format;
import index_format;
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

			vertex_buffer(const logical_device&,
						  const memory_allocator&,
						  const std::vector<vertex>&,
						  const std::vector<vertex_index_t>&,
						  const create_info& = {});

			auto vertices() const -> const buffer_subdata;
			auto indices() const -> const buffer_subdata;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			mapped_buffer m_vertex_and_index_buffer;
			buffer_subdata m_vertex_and_index_suballocations;
		};
	}
}
