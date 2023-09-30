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
import vulkan;
#endif

export namespace lh
{
	namespace vulkan
	{
		class queue_families
		{
		public:
			struct queue
			{
				using index_t = std::uint32_t;
				using priority_t = float;

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