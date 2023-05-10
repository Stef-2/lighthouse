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
			using index_t = decltype(vk::QueueFamilyProperties::queueCount);

			struct create_info
			{};

			queue_families(const physical_device&, const surface&, const create_info& = {});

			auto graphics_index() const -> const index_t&;
			auto present_index() const -> const index_t&;
			auto compute_index() const -> const index_t&;
			auto transfer_index() const -> const index_t&;

		private:
			index_t m_graphics;
			index_t m_present;
			index_t m_compute;
			index_t m_transfer;
		};
	}
}
