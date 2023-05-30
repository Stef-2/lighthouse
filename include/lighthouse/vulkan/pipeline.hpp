#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

#include <vector>

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;
		class memory_allocator;
		class descriptor_set_layout;
		class spir_v;
		class shader_object;
		class descriptor_collection;

		class pipeline
		{
		public:
			struct create_info
			{};

			pipeline(const physical_device&,
					 const logical_device&,
					 const memory_allocator&,
					 const std::vector<std::pair<spir_v, descriptor_set_layout>>&,
					 const create_info& = {});

			auto bind() const -> void;

		private:
			vk::raii::PipelineLayout m_pipeline_layout;

			std::vector<shader_object> m_shaders;
			std::vector<descriptor_collection> m_descriptor_collections;
		};
	}
}
