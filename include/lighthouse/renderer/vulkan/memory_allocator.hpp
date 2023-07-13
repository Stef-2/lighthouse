#pragma once

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#else
import raii_wrapper;
#endif

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class instance;
		class physical_device;
		class logical_device;

		class memory_allocator : public raii_wrapper<vma::Allocator>
		{
		public:
			struct create_info
			{};

			memory_allocator(const instance&, const physical_device&, const logical_device&, const create_info& = {});
			~memory_allocator();

		private:
		};
	}
}
