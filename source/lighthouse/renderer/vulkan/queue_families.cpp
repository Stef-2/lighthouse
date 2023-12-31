module;

#include <limits>
#include <compare>

module queue;

namespace lh
{
	namespace vulkan
	{
		queue_families::queue_families(const physical_device& physical_device,
									   const surface& surface,
									   const create_info& create_info)
			: m_graphics {std::numeric_limits<family::index_t>::max(), create_info.m_graphics},
			  m_present {std::numeric_limits<family::index_t>::max(), create_info.m_present},
			  m_compute {std::numeric_limits<family::index_t>::max(), create_info.m_compute},
			  m_transfer {std::numeric_limits<family::index_t>::max(), create_info.m_transfer},
			  m_combined_graphics_and_present_family {},
			  m_dedicated_compute_family {},
			  m_dedicated_transfer_family {}
		{
			const auto queue_family_properties = physical_device->getQueueFamilyProperties2();
			auto counter = queue_families::family::index_t {};

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

			for (family::index_t i {}; i < std::numeric_limits<family::index_t>::max(); i++)
				if (physical_device->getSurfaceSupportKHR(i, **surface))
				{
					m_present.m_index = i;
					break;
				}

			if (m_graphics.m_index == m_present.m_index)
				m_combined_graphics_and_present_family = true;
			if (m_graphics.m_index != m_compute.m_index)
				m_dedicated_compute_family = true;
			if (m_graphics.m_index != m_transfer.m_index)
				m_dedicated_transfer_family = true;
		}

		auto queue_families::graphics() const -> const family&
		{
			return m_graphics;
		}

		auto queue_families::present() const -> const family&
		{
			return m_present;
		}

		auto queue_families::compute() const -> const family&
		{
			return m_compute;
		}

		auto queue_families::transfer() const -> const family&
		{
			return m_transfer;
		}

		auto queue_families::supports_combined_graphics_and_present_family() const -> const bool&
		{
			return m_combined_graphics_and_present_family;
		}

		auto queue_families::supports_dedicated_compute_family() const -> const bool&
		{
			return m_dedicated_compute_family;
		}

		auto queue_families::supports_dedicated_transfer_family() const -> const bool&
		{
			return m_dedicated_transfer_family;
		}
	}
}
