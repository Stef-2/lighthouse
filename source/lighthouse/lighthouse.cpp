#include "vkfw/vkfw.hpp"

import engine;
import window;
import bounding_volume;

import std;

auto main() -> int
{
	lh::geometry::octree o {{{0.0f, 0.0f, 0.0f}, {8.0f, 8.0f, 8.0f}}, 0.1};

	// auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	// engine.run();
}
