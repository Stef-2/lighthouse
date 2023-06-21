#pragma once

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;
		class queue_families;

		class queue
		{

		public:
			struct create_info
			{};

			queue(const logical_device&, const queue_families&, const create_info& = {});

			auto graphics() const -> const vk::raii::Queue&;
			auto present() const -> const vk::raii::Queue&;

		private:
			vk::raii::Queue m_graphics;
			vk::raii::Queue m_present;
		};
	}
}
