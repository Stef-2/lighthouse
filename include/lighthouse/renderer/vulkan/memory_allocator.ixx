module;

#include "vma/vk_mem_alloc.hpp"

export module memory_allocator;

import raii_wrapper;
import instance;
import physical_device;
import logical_device;

export namespace lh
{
	namespace vulkan
	{
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

export namespace vma
{
	using vma::Allocator;
	using vma::AllocatorCreateFlags;
	using vma::AllocatorCreateFlagBits;

	using vma::Allocation;
	using vma::AllocationCreateInfo;
	using vma::AllocationCreateFlags;
	using vma::AllocationCreateFlagBits;
	using vma::AllocationInfo;

	using vma::MemoryUsage;
}