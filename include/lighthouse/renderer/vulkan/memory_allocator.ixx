module;
#pragma once

export module memory_allocator;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/renderer/vulkan/instance.ixx"
#include "lighthouse/renderer/vulkan/physical_device.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#else
import raii_wrapper;
import instance;
import physical_device;
import logical_device;
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

		private:
		};
	}
}