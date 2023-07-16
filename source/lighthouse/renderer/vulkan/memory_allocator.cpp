module;

#define VMA_IMPLEMENTATION

#include <ranges>

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/memory_allocator.ixx"
#else
module memory_allocator;
#endif

import output;

namespace lh
{
	namespace vulkan
	{

		memory_allocator::memory_allocator(const instance& instance,
										   const physical_device& physical_device,
										   const logical_device& logical_device,
										   const create_info& create_info)
		{
			auto allocator_extensions = vma::AllocatorCreateFlags {};

			const auto assert_version_above_1_2 = instance.version() >= lh::version {1, 2, 0};

			if (std::ranges::contains(physical_device.extensions().required_extensions(),
									  VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
				allocator_extensions |= vma::AllocatorCreateFlagBits::eExtMemoryBudget;

			if (std::ranges::contains(physical_device.extensions().required_extensions(),
									  VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) or
				assert_version_above_1_2)

				allocator_extensions |= vma::AllocatorCreateFlagBits::eBufferDeviceAddress;

			const auto allocator_info = vma::AllocatorCreateInfo {allocator_extensions,
																  **physical_device,
																  **logical_device,
																  0,
																  nullptr,
																  nullptr,
																  nullptr,
																  nullptr,
																  **instance,
																  instance.version(),
																  nullptr};

			auto result = vma::createAllocator(&allocator_info, &m_object);

			if (result != vk::Result::eSuccess)
				output::fatal() << "unable to initialize vulkan memory allocator";
		}

		memory_allocator::~memory_allocator()
		{
			m_object.destroy();
		}
	}
}
