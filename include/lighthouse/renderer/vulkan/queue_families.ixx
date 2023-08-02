module;
#pragma once

export module queue_families;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/physical_device.ixx"
#include "lighthouse/renderer/vulkan/surface.ixx"
#else
import physical_device;
import surface;
#endif

import std.core;
import vulkan;

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
