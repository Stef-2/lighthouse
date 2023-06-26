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
	}

	class pipeline_resource_genertor
	{
		using pipeline_spir_v_code = std::vector<vulkan::spir_v>;

		struct create_info
		{};

		pipeline_resource_genertor(const vulkan::physical_device&,
								   const vulkan::logical_device&,
								   const vulkan::memory_allocator&,
								   const pipeline_spir_v_code&,
								   const create_info& = {});
	};
}
