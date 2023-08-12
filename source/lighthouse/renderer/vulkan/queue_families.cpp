module;

#include <compare>

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/queue.ixx"
#else
module queue;
#endif

namespace lh
{
	namespace vulkan
	{
		queue_families::queue_families(const physical_device& physical_device,
									   const surface& surface,
									   const create_info& create_info)
			: m_graphics {create_info.m_graphics},
			  m_present {create_info.m_present},
			  m_compute {create_info.m_compute},
			  m_transfer {create_info.m_transfer}
		{
			const auto queue_family_properties = physical_device->getQueueFamilyProperties2();
			auto counter = queue_families::queue::index_t {};

			for (const auto& queue_family_property : queue_family_properties)
			{
				if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics)
					m_graphics.m_index = counter;
				if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute)
					m_compute.m_index = counter;
				if (queue_family_property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eTransfer)
					m_transfer.m_index = counter;

				counter++;
			}

			for (queue_families::queue::index_t i {}; i < std::numeric_limits<queue_families::queue::index_t>::max();
				 i++)
				if (physical_device->getSurfaceSupportKHR(0, **surface))
				{
					m_present.m_index = i;
					break;
				}
		}

		auto queue_families::graphics() const -> const queue&
		{
			return m_graphics;
		}

		auto queue_families::present() const -> const queue&
		{
			return m_present;
		}

		auto queue_families::compute() const -> const queue&
		{
			return m_compute;
		}

		auto queue_families::transfer() const -> const queue&
		{
			return m_transfer;
		}
	}
}
