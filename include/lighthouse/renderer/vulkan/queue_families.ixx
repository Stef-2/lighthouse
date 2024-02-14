module;

#if INTELLISENSE
#include <cstdint>
#endif

#include <compare>

export module queue_families;

import physical_device;
import surface;

#if not INTELLISENSE
import std.core;
import vulkan_hpp;
#endif

export namespace lh
{
	namespace vulkan
	{
		class queue_families
		{
		public:
			struct family
			{
				using index_t = std::uint32_t;
				using priority_t = float;

				index_t m_index {};
				priority_t m_priority {1.0};
			};

			struct create_info
			{
				family::priority_t m_graphics = {};
				family::priority_t m_present = {};
				family::priority_t m_compute = {};
				family::priority_t m_transfer = {};
			};

			queue_families(const physical_device&, const surface&, const create_info& = {});

			auto graphics() const -> const family&;
			auto present() const -> const family&;
			auto compute() const -> const family&;
			auto transfer() const -> const family&;

			auto supports_combined_graphics_and_present_family() const -> const bool&;
			auto supports_dedicated_compute_family() const -> const bool&;
			auto supports_dedicated_transfer_family() const -> const bool&;

		private:
			family m_graphics;
			family m_present;
			family m_compute;
			family m_transfer;

			bool m_combined_graphics_and_present_family;
			bool m_dedicated_compute_family;
			bool m_dedicated_transfer_family;
		};
	}
}