#include "vkfw/vkfw.hpp"

import engine;
import window;

import memory_pool;
import time;

import std;

auto main() -> int
{
	lh::memory_pool pool(100'000'0000);

	auto la = pool.allocator<int>();
	auto mla = pool.allocator<std::pair<const int, int>>();

	auto pv = new std::vector<int, decltype(la)>(la);
	auto v = new std::vector<int>();

	auto pl = new std::list<int, decltype(la)> {la};
	auto l = new std::list<int> {};

	auto pd = new std::deque<int, decltype(la)> {la};
	auto d = new std::deque<int> {};

	auto ps = new std::set<int, std::less<int>, decltype(la)> {la};
	auto s = new std::set<int> {};

	auto pm = new std::map<const int, int, std::less<int>, decltype(mla)> {mla};
	auto m = new std::map<const int, int> {};

	auto pus = new std::unordered_set<int, std::hash<int>, std::equal_to<int>, decltype(la)> {la};
	auto us = new std::unordered_set<int> {{1}};

	std::srand(std::time(nullptr));
	size_t wtf = 1'000'000;
	// rand() % 10'000'000 + 5'000'000;
	std::cout << "num iters: " << wtf << '\n';

	lh::time::stopwatch sw1 {};
	for (size_t i = 0; i < wtf; i++)
	{
		v->push_back(i);
		// l->push_back(i);
		// d->push_back(i);
		// s->insert(i);
		// us->insert(i);
		//  m->emplace(i, i);
	}
	auto dur1 = sw1.stop();

	std::cout << dur1 << " ms\n";

	lh::time::stopwatch sw2 {};
	for (size_t i = 0; i < wtf; i++)
	{
		pv->push_back(i);
		//pl->push_back(i);
		//pd->push_back(i);
		//ps->insert(i);
		//pus->insert(i);
		// pm->emplace(i, i);
	}
	auto dur2 = sw2.stop();

	std::cout << dur2 << " ms\n";
	std::cout << "ratio: " << (float)dur2 / (float)dur1 << '\n';

	delete pv;
	delete pl;
	delete pd;
	delete ps;
	delete pus;
	delete pm;

	// std::cout << v.back() << ' ' << pv.back() << '\n';
	//  auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	// engine.run();
}
