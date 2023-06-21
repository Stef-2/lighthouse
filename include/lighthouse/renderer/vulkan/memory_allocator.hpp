#pragma once

#include "lighthouse/renderer/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class instance;
		class physical_device;
		class logical_device;

		class memory_allocator : public vk_wrapper<vma::Allocator>
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
