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
		class buffer;

		class descriptor_collection
		{
		public:
			struct create_info
			{};

			descriptor_collection(const physical_device&,
								  const logical_device&,
								  const descriptor_set_layout&,
								  const memory_allocator&,
								  const create_info& = {});

			auto descriptor_buffers() const -> const std::vector<std::pair<buffer, void*>>&;

		private:
			std::vector<std::pair<buffer, void*>> m_descriptor_buffers;
		};
	}
}
