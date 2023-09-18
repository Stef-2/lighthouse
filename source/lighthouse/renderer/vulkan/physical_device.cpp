module;

module physical_device;
import output;
import lh_memory;

namespace lh
{
	namespace vulkan
	{

		lh::vulkan::physical_device::physical_device(const instance& instance, const create_info& create_info)
			: m_extensions {preferred_device(instance, create_info).enumerateDeviceExtensionProperties(),
							create_info.m_extensions},
			  m_performance_score {0},
			  m_properties {},
			  m_features {}
		{
			m_object = preferred_device(instance, create_info);
			m_performance_score = performance_score(m_object);

			// physical device properties
			auto host_image_properties = vk::PhysicalDeviceHostImageCopyPropertiesEXT {};
			auto physical_device_properties = vk::PhysicalDeviceProperties2 {};
			physical_device_properties.pNext = &host_image_properties;

			m_object.getDispatcher()->vkGetPhysicalDeviceProperties2(*m_object,
																	 &static_cast<VkPhysicalDeviceProperties2&>(
																		 physical_device_properties));

			auto host_image_source_layouts = std::vector<vk::ImageLayout>(host_image_properties.copySrcLayoutCount);
			auto host_image_destination_layouts = std::vector<vk::ImageLayout>(
				host_image_properties.copyDstLayoutCount);

			host_image_properties = vk::PhysicalDeviceHostImageCopyPropertiesEXT {host_image_source_layouts,
																				  host_image_destination_layouts};

			m_object.getDispatcher()->vkGetPhysicalDeviceProperties2(*m_object,
																	 &static_cast<VkPhysicalDeviceProperties2&>(
																		 physical_device_properties));

			const auto properties = m_object.getProperties2<vk::PhysicalDeviceProperties2,
															vk::PhysicalDeviceShaderObjectPropertiesEXT,
															vk::PhysicalDeviceDescriptorIndexingProperties,
															vk::PhysicalDeviceMaintenance5PropertiesKHR,
															vk::PhysicalDeviceDescriptorBufferPropertiesEXT,
															vk::PhysicalDeviceHostImageCopyPropertiesEXT>();

			m_properties = {properties.get<vk::PhysicalDeviceProperties2>(),
							properties.get<vk::PhysicalDeviceShaderObjectPropertiesEXT>(),
							properties.get<vk::PhysicalDeviceDescriptorIndexingProperties>(),
							properties.get<vk::PhysicalDeviceMaintenance5PropertiesKHR>(),
							{properties.get<vk::PhysicalDeviceDescriptorBufferPropertiesEXT>()},
							{host_image_properties, host_image_source_layouts, host_image_destination_layouts}};

			m_properties.m_host_image_copy_properties.m_properties.pCopySrcLayouts =
				m_properties.m_host_image_copy_properties.m_source_layouts.data();
			m_properties.m_host_image_copy_properties.m_properties.pCopyDstLayouts =
				m_properties.m_host_image_copy_properties.m_destination_layouts.data();

			// physical device features
			const auto features = m_object.getFeatures2<vk::PhysicalDeviceFeatures2,
														vk::PhysicalDeviceDynamicRenderingFeatures,
														vk::PhysicalDeviceShaderObjectFeaturesEXT,
														vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT,
														vk::PhysicalDeviceDescriptorIndexingFeatures,
														vk::PhysicalDeviceDescriptorBufferFeaturesEXT,
														vk::PhysicalDeviceMaintenance5FeaturesKHR,
														vk::PhysicalDeviceHostImageCopyFeaturesEXT>();

			m_features = {features.get<vk::PhysicalDeviceFeatures2>(),
						  features.get<vk::PhysicalDeviceDynamicRenderingFeatures>(),
						  features.get<vk::PhysicalDeviceShaderObjectFeaturesEXT>(),
						  features.get<vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT>(),
						  features.get<vk::PhysicalDeviceDescriptorIndexingFeatures>(),
						  features.get<vk::PhysicalDeviceDescriptorBufferFeaturesEXT>(),
						  features.get<vk::PhysicalDeviceMaintenance5FeaturesKHR>(),
						  features.get<vk::PhysicalDeviceHostImageCopyFeaturesEXT>()};
		}

		auto lh::vulkan::physical_device::extensions() const -> physical_extensions
		{
			return m_extensions;
		}

		auto lh::vulkan::physical_device::info() const -> lh::string::string_t
		{
			const auto properties = m_object.getProperties2();
			const auto memory = memory::physical_device_memory(m_object);
			constexpr auto gigabyte = static_cast<double>(1_gb);

			auto info = string::string_t {"\n======== basic vulkan device information: ========"};
			info += "\n\tname: " + string::string_t {properties.properties.deviceName.data()};
			info += "\n\tapi version: " + lh::string::string_t(lh::version {properties.properties.apiVersion});
			info += "\n\tdriver version: " + lh::string::string_t(lh::version {properties.properties.driverVersion});
			info += "\n\ttotal memory: " + std::to_string(double(memory.m_device_total) / gigabyte) + " gygabites";
			info += "\n\tavailable memory: " + std::to_string(double(memory.m_device_available) / gigabyte) +
					" gygabites";
			info += "\n\tused memory: " + std::to_string(double(memory.m_device_used) / gigabyte) + " gigabytes";
			info += "\n\tfree memory: " + std::to_string(memory.m_device_used_percentage) + " %\n";

			info += string::string_t {"\n======== advanced vulkan device information: ========"};
			info += "\n\tmax bound descriptor sets: " +
					std::to_string(properties.properties.limits.maxBoundDescriptorSets);
			info += "\n\tmax push constant size: " + std::to_string(properties.properties.limits.maxPushConstantsSize) +
					" bytes\n";

			return info;
		}

		auto lh::vulkan::physical_device::performance_score(const vk::raii::PhysicalDevice& device)
			-> performance_score_t
		{
			const auto properties = device.getProperties2();
			const auto features = device.getFeatures2();

			constexpr auto gigabyte = static_cast<double>(1_gb);
			const auto memory = memory::physical_device_memory(device);

			auto score = performance_score_t {};

			score += memory.m_device_total + memory.m_shared_total;
			score += properties.properties.limits.maxImageDimension2D;
			score += performance_score_t {properties.properties.limits.maxFramebufferWidth *
										  properties.properties.limits.maxFramebufferHeight};

			return score;
		}

		auto lh::vulkan::physical_device::properties() const -> const physical_properties&
		{
			return m_properties;
		}

		auto physical_device::features() const -> const physical_features&
		{
			return m_features;
		}

		auto lh::vulkan::physical_device::performance_score() const -> performance_score_t
		{
			return m_performance_score;
		}

		auto lh::vulkan::physical_device::preferred_device(const instance& instance, const create_info& create_info)
			-> vk::raii::PhysicalDevice
		{
			// enumerate all vulkan capable physical devices
			auto physical_devices = instance->enumeratePhysicalDevices();

			// assert that there are any vulkan capable devices
			if (physical_devices.empty())
				output::fatal() << "this system does not support any vulkan capable devices";

			// sort them according to their performance score
			std::ranges::sort(physical_devices,
							  [](const auto& x, const auto& y) { return performance_score(x) < performance_score(y); });

			// assert that the device with the highest score is above the minimum score threshold
			auto& strongest_device = physical_devices.front();
			if (performance_score(strongest_device) < create_info.m_minimum_accepted_score)
				output::fatal() << "this system does not have any suitable vulkan devices";

			return std::move(strongest_device);
		}
	}
}
