#pragma once

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class physical_device;
		class surface;

		class queue_families
		{
		public:
			struct queue
			{
				using index_t = decltype(vk::DeviceQueueCreateInfo::queueFamilyIndex);
				using priority_t = decltype(*vk::DeviceQueueCreateInfo::pQueuePriorities);

				index_t m_index {};
				priority_t m_priority {1.0};
			};

			struct create_info
			{
				queue m_graphics = {};
				queue m_present = {};
				queue m_compute = {};
				queue m_transfer = {};
			};

			queue_families(const physical_device&, const surface&, const create_info& = {});

			auto graphics() const -> const queue&;
			auto present() const -> const queue&;
			auto compute() const -> const queue&;
			auto transfer() const -> const queue&;

		private:
			queue m_graphics;
			queue m_present;
			queue m_compute;
			queue m_transfer;
		};
	}
}
