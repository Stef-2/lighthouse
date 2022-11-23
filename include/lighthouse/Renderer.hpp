#pragma once

#include "vkfw.hpp"

namespace lh
{
	class renderer
	{
	public:

	private:

		vk::raii::SurfaceKHR vkSurface;
		vk::raii::Instance vkInstance;
	};
}