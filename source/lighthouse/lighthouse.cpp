#include "vkfw/vkfw.hpp"
/*
#if INTELLISENSE
#include "lighthouse/engine.ixx"
import std;
#else
#endif*/
import engine;
import window;
import test123;

auto main() -> int
{
	test_func();
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {640, 420}})};

	engine.run();
}
