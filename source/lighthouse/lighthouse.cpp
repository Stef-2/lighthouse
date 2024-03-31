#include "vkfw/vkfw.hpp"

import engine;
import window;
import physical_property;
import geometry;
import std.core;
auto main() -> int
{
	// lh::geometry::ray r {{0.0f, 0.0f, 0.0f}, {0.33f, 0.33f, 0.33f}};

	// lh::geometry::triangle t {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
	// auto test = lh::geometry::ray_tri_test(r, t);
	// std::cout << test.u << '\n';
	// std::exit(0);
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	engine.run();
}
