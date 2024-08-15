module;

module lh_memory;

auto lh::memory::physical_device_memory(const vk::raii::PhysicalDevice& physical_device)
	-> memory::physical_device_memory_info
{
	const auto memory = physical_device.getMemoryProperties2<vk::PhysicalDeviceMemoryProperties2,
															 vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();
	const auto [memory_properties, memory_budget] =
		memory.get<vk::PhysicalDeviceMemoryProperties2, vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();

	auto device_total = vk::DeviceSize {};
	auto device_available = vk::DeviceSize {};
	auto device_used = vk::DeviceSize {};

	auto shared_total = vk::DeviceSize {};
	auto shared_available = vk::DeviceSize {};
	auto shared_used = vk::DeviceSize {};

	const auto heap_count = memory_properties.memoryProperties.memoryHeapCount;

	for (auto i = std::size_t {}; i < heap_count; i++)
	{
		if (memory_properties.memoryProperties.memoryHeaps[i].flags == vk::MemoryHeapFlagBits::eDeviceLocal)
		{
			device_total += memory_properties.memoryProperties.memoryHeaps[i].size;
			device_available += memory_budget.heapBudget[i];
		} else
		{
			shared_total += memory_properties.memoryProperties.memoryHeaps[i].size;
			shared_available += memory_budget.heapBudget[i];
		}
	}

	device_used = device_total - device_available;
	shared_used = shared_total - shared_available;

	auto device_available_percentage = static_cast<double>(device_available) / static_cast<double>(device_total);
	auto shared_available_percentage = static_cast<double>(shared_available) / static_cast<double>(shared_total);

	return {device_total,
			device_available,
			device_used,
			device_available_percentage,
			shared_total,
			shared_available,
			shared_used,
			shared_available_percentage};
}
