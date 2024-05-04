// #include "vkfw/vkfw.hpp"

import engine;
import window;

#if not INTELLISENSE
import std.core;
#endif

auto main() -> int
{

	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	engine.run();
}
