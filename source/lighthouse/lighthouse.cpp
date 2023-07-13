#include "lighthouse/lighthouse.hpp"

#if INTELLISENSE
#include "lighthouse/engine.ixx"
#else
import engine;
#endif

auto main() -> int
{
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {640, 420}})};

	engine.run();
}
