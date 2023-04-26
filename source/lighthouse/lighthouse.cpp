#include "lighthouse/lighthouse.hpp"
#include "lighthouse/filesystem.hpp"
#include "lighthouse/input.hpp"

auto main() -> void
{
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {640, 420}})};

	engine.run();
}
