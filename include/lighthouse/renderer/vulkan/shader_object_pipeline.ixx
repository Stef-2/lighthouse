module;
#pragma once

#include <vector>

export module shader_object_pipeline;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/physical_device.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/memory_allocator.ixx"
#include "lighthouse/renderer/vulkan/descriptor_set_layout.ixx"
#include "lighthouse/renderer/vulkan/shader_object.ixx"
#include "lighthouse/renderer/vulkan/descriptor_buffer.ixx"
#include "lighthouse/renderer/vulkan/spir_v.ixx"
#include "lighthouse/renderer/vulkan/vertex_input_description.ixx"
#else
import physical_device;
import logical_device;
import memory_allocator;
import descriptor_set_layout;
import shader_object;
import descriptor_buffer;
import spir_v;
import vertex_input_description;
#endif

import vulkan;

export namespace lh
{
	namespace vulkan
	{
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
								   const create_info& = {});

			auto shader_objects() const -> const std::vector<shader_object>&;
			auto pipeline_layout() const -> const vk::raii::PipelineLayout&;
			auto descriptor_set_layout() const -> const descriptor_set_layout&;
			auto descriptor_buffer() const -> const descriptor_buffer&;
			auto vertex_input_description() const -> const vertex_input_description&;
			auto bind(const vk::raii::CommandBuffer&) const -> void;

		private:
			vulkan::descriptor_set_layout m_descriptor_set_layout;
			//vulkan::descriptor_buffer m_descriptor_buffer;
			vulkan::vertex_input_description m_vertex_input_description;
			vk::raii::PipelineLayout m_pipeline_layout;
			std::vector<shader_object> m_shader_objects;
		};
	}
};
