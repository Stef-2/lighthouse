#include "vkfw/vkfw.hpp"

import engine;
import window;
import physical_property;
auto main() -> int
{
	lh::geometry::func();
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	engine.run();
}
