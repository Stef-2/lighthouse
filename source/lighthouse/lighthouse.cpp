#include "lighthouse/lighthouse.hpp"

#if INTELLISENSE
#include "lighthouse/file_system.ixx"
#else
import file_system;
#endif

#include "lighthouse/input.hpp"

auto main() -> const void
{
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {640, 420}})};

	engine.run();
}
