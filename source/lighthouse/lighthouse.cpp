﻿#include "lighthouse.hpp"

int main()
{
	auto engine = lh::engine {std::make_unique<window>(window::create_info {.m_resolution {640, 320}})};

	engine.run();
}
