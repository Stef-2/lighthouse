#include "lighthouse/lighthouse.hpp"
#include "lighthouse/filesystem.hpp"
#include "lighthouse/input.hpp"

auto main() -> const void
{
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {640, 420}})};

	lh::node root;
	lh::node ch(root);

	std::cout << root.descendent_count();

	engine.run();
}
