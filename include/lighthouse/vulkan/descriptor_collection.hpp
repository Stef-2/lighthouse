#pragma once

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;
		class memory_allocator;
		class descriptor_set_layout;

		class descriptor_collection
		{
		public:
			struct descriptor
			{};

			struct create_info
			{
				vk::ShaderCreateFlagsEXT m_flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;
				vk::ShaderCodeTypeEXT m_code_type = vk::ShaderCodeTypeEXT::eSpirv;
			};

			descriptor_collection(const physical_device&,
								  const logical_device&,
								  const descriptor_set_layout&,
								  const memory_allocator&,
								  const create_info& = {});

		private:
			vk::raii::ShaderEXTs m_shaders;
		};
	}
}
