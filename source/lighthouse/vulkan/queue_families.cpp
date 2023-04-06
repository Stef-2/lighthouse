#include "lighthouse/vulkan/queue_families.hpp"
#include "lighthouse/vulkan/physical_device.hpp"
#include "lighthouse/vulkan/surface.hpp"

lh::vulkan::queue_families::queue_families(const physical_device& physical_device,
										   const surface& surface,
										   const create_info& create_info)
	: m_graphics {}, m_present {}, m_compute {}, m_transfer {}
{
	const auto queue_family_properties = physical_device->getQueueFamilyProperties2();
	auto counter = queue_families::index_t {};

	for (const auto& queue_family_property : queue_family_properties)
	{
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
			m_graphics = counter;
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute)
			m_compute = counter;
		if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eTransfer)
			m_transfer = counter;

		counter++;
	}

	for (queue_families::index_t i {}; i < std::numeric_limits<queue_families::index_t>::max(); i++)
		if (physical_device->getSurfaceSupportKHR(0, **surface))
		{
			m_present = i;
			break;
		}
}

auto lh::vulkan::queue_families::graphics_index() const -> const index_t&
{
	return m_graphics;
}

auto lh::vulkan::queue_families::present_index() const -> const index_t&
{
	return m_present;
}

auto lh::vulkan::queue_families::compute_index() const -> const index_t&
{
	return m_compute;
}

auto lh::vulkan::queue_families::transfer_index() const -> const index_t&
{
	return m_transfer;
}
