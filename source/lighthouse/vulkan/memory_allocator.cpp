#include "lighthouse/vulkan/memory_allocator.hpp"
#include "lighthouse/vulkan/instance.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/logical_device.hpp"

#define VMA_IMPLEMENTATION

lh::vulkan::memory_allocator::memory_allocator(const instance& instance,
											   const physical_device& physical_device,
											   const logical_device& logical_device,
											   const create_info& create_info)
{
	auto allocator_info = vma::AllocatorCreateInfo {{},
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

lh::vulkan::memory_allocator::~memory_allocator()
{
	m_object.destroy();
}
