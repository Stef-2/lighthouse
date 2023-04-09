#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	// forward declarations
	class window;
	namespace vulkan
	{
		class instance;
	}

	namespace vulkan
	{
		class surface : public vk_wrapper<vk::raii::SurfaceKHR>
		{
		public:
			struct create_info
			{};

			surface(const lh::window&, const instance&, const create_info& = {});

			auto area() const -> const vk::Rect2D&;

		private:
			vk::Rect2D m_area;
		};
	}
}
