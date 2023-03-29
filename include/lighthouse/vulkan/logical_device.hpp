#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"
#include "lighthouse/vulkan/string.hpp"
#include "lighthouse/string/string.hpp"

#include <vector>

namespace lh
{
	namespace vulkan
	{

		// forward declarations
		class physical_device;

		class logical_device : public vk_wrapper<vk::raii::Device>
		{
		public:
			struct create_info
			{
				std::vector<vk::DeviceQueueCreateInfo> m_queues;
				vk::PhysicalDeviceFeatures2 m_features = vk::PhysicalDeviceFeatures2 {};
				vk_string_t m_extensions;
			};

			logical_device(const physical_device&, const create_info&);

			auto info() const -> lh::string::string_t override;

		private:
		};
	}
}
