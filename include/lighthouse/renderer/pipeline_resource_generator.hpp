#pragma once

#include <vector>

namespace lh
{
	// forward declrations
	namespace vulkan
	{
		class physical_device;
		class logical_device;
		class memory_allocator;
		class descriptor_set_layout;
		class descriptor_buffer;
		class shader_object;
		class vertex_buffer;
		class mapped_buffer;
		class spir_v;

		class pipeline_resource_genertor
		{
			using pipeline_spir_v_code = std::vector<vulkan::spir_v>;

			struct shader_resources
			{
				std::vector<shader_object> m_shader_objects;
				std::vector<descriptor_set_layout> m_descriptor_set_layout;
				vk::raii::PipelineLayout m_pipeline_layout;

				vk::VertexInputBindingDescription2EXT m_bindings;
				std::vector<vk::VertexInputAttributeDescription2EXT> m_attributes;
			};

			struct create_info
			{};

			pipeline_resource_genertor(const physical_device&,
									   const logical_device&,
									   const memory_allocator&,
									   const pipeline_spir_v_code&,
									   const create_info& = {});
		};
	}
}
