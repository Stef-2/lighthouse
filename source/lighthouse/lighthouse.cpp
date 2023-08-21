#include "vkfw/vkfw.hpp"

import engine;
import window;

auto main() -> int
{
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {640, 420}})};

	engine.run();
}
