#include "vkfw/vkfw.hpp"

import engine;
import window;

import memory_pool;
import time;

import std;

auto main() -> int
{

	lh::memory_pool pool(640'000'0000);

	auto la = pool.allocator<int>();

	auto pv = new std::vector<int, decltype(la)>(la);
	auto v = new std::vector<int>();

	auto pl = new std::list<int, decltype(la)> {la};
	auto l = new std::list<int> {};

	auto pd = new std::deque<int, decltype(la)> {la};
	auto d = new std::deque<int> {};

	std::srand(std::time(nullptr));
	size_t wtf = 10000000;
	// rand() % 10'000'000 + 5'000'000;
	std::cout << "num iters: " << wtf << '\n';

	lh::time::stopwatch sw1 {};
	for (size_t i = 0; i < wtf; i++)
	{
		v->push_back(i * 3);
		l->push_back(i * 4);
		d->push_back(i * 5);
	}
	auto dur1 = sw1.stop();

	std::cout << dur1 << " ms\n";

	lh::time::stopwatch sw2 {};
	for (size_t i = 0; i < wtf; i++)
	{
		pv->push_back(i * 3);
		pl->push_back(i * 4);
		pd->push_back(i * 5);
	}
	auto dur2 = sw2.stop();

	std::cout << dur2 << " ms\n";
	std::cout << "ratio: " << (float)dur2 / (float)dur1 << '\n';

	delete pv;
	delete pl;
	delete pd;

	// std::cout << v.back() << ' ' << pv.back() << '\n';
	//  auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	// engine.run();
}
