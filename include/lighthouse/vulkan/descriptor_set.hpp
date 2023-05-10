#pragma once

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class logical_device;
		class memory_allocator;

		class descriptor_set
		{
		public:
			struct descriptor
			{};

			struct create_info
			{};

			descriptor_set(const physical_device&,
						   const logical_device&,
						   const memory_allocator&,
						   const create_info& = {});

		private:
		};
	}
}
