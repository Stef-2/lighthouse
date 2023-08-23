#include "vkfw/vkfw.hpp"

#include <windows.h>

import engine;
import window;

auto main() -> int
{
	// Sleep(10000);

	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {640, 420}})};

	engine.run();
}
