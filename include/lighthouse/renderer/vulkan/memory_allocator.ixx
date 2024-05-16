module;

#if INTELLISENSE
#include "vma/vk_mem_alloc.hpp"
#endif

export module memory_allocator;

import raii_wrapper;
import instance;
import physical_device;
import logical_device;

#if not INTELLISENSE
export import vk_mem_alloc_hpp;
#endif

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

			auto statistics() const -> const vma::TotalStatistics;
		private:
		};
	}
}