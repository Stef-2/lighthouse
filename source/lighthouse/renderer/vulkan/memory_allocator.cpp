module;

#define VMA_IMPLEMENTATION

#include "vulkan/vulkan_core.h"
#include "vma/vk_mem_alloc.hpp"

module memory_allocator;
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

			const auto assert_version_gte_1_2 = instance.version() >= lh::version {1, 2, 0};
			const auto assert_version_gte_1_3 = instance.version() >= lh::version {1, 3, 0};

			if (std::ranges::contains(physical_device.extensions().required_extensions(),
									  VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
				allocator_extensions |= vma::AllocatorCreateFlagBits::eExtMemoryBudget;

			if (std::ranges::contains(physical_device.extensions().required_extensions(),
									  VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME))
				allocator_extensions |= vma::AllocatorCreateFlagBits::eExtMemoryPriority;

			if (std::ranges::contains(physical_device.extensions().required_extensions(),
									  VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) or
				assert_version_gte_1_2)
			{
				allocator_extensions |= vma::AllocatorCreateFlagBits::eBufferDeviceAddress;
				allocator_extensions |= vma::AllocatorCreateFlagBits::eKhrBindMemory2;
			}

			if (std::ranges::contains(physical_device.extensions().required_extensions(),
									  VK_KHR_MAINTENANCE_4_EXTENSION_NAME) or
				assert_version_gte_1_3)
				allocator_extensions |= vma::AllocatorCreateFlagBits::eKhrMaintenance4;

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

			if (result != vk::Result::eSuccess) output::fatal() << "unable to initialize vulkan memory allocator";
		}

		memory_allocator::~memory_allocator()
		{
			m_object.destroy();
		}

		auto memory_allocator::statistics() const -> const vma::TotalStatistics
		{
			return m_object.calculateStatistics();
		}
		auto memory_allocator::budget() const -> const const std::vector<vma::Budget>
		{
			return m_object.getHeapBudgets();
		}
	}
}
