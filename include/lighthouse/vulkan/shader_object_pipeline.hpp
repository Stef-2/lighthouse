#pragma once

#include "lighthouse/vulkan/descriptor_collection.hpp"

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
		class shader_object;
		class spir_v;

		class shader_object_pipeline
		{
		public:
			struct create_info
			{
				vk::ShaderCreateFlagsEXT m_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
			};

			shader_object_pipeline(const physical_device&,
								   const logical_device&,
								   const memory_allocator&,
								   const std::vector<spir_v>&,
								   const descriptor_set_layout&,
								   const create_info& = {});

			auto shader_objects() const -> const std::vector<shader_object>&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			vk::raii::PipelineLayout m_pipeline_layout;
			std::vector<shader_object> m_shader_objects;
			descriptor_collection m_descriptor_collection;
		};
	}
};
