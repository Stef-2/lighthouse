#include "lighthouse/lighthouse.hpp"
#include "lighthouse/filesystem.hpp"
#include "lighthouse/input.hpp"

auto main() -> void
{
	auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {640, 420}})};

	const auto shader = lh::input::read_file(lh::file_system::find("CMakeLists.txt"));

	auto bytes = lh::input::read_file<lh::input::file_type::binary>(lh::file_system::find("CMakeLists.txt"));
	std::cout << bytes.size();

	for (std::byte& byte : bytes)
		std::cout << int(byte);

	engine.run();
}
