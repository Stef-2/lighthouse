#include "vkfw/vkfw.hpp"

import engine;
import window;
import object_index;

#if not INTELLISENSE
import std.core;
#endif

import memory_suballocator;

template <typename T>
class pool_allocator
{
public:
	using value_type = T;

	template <typename>
	friend class pool_allocator;

	pool_allocator(lh::worst_fit_suballocator& memory_suballocator) : m_memory_suballocator {memory_suballocator} {}

	template <class Y>
	pool_allocator(const pool_allocator<Y>& other) noexcept : m_memory_suballocator {other.m_memory_suballocator}
	{}
	template <class Y>
	bool operator==(const pool_allocator<Y>&) const noexcept
	{
		return true;
	}
	template <class Y>
	bool operator!=(const pool_allocator<Y>&) const noexcept
	{
		return false;
	}

	T* allocate(const size_t element_count)
	{
		const auto suballocation = m_memory_suballocator.request_and_commit_suballocation(element_count * sizeof T);
		return static_cast<T*>(*suballocation);
	}

	void deallocate(T* const pointer, size_t element_count) noexcept
	{
		m_memory_suballocator.free_suballocation({reinterpret_cast<std::size_t>(pointer), element_count * sizeof T});
	}

private:
	lh::worst_fit_suballocator& m_memory_suballocator;
};

auto main() -> int
{
	[[assume(true == false)]]
	const auto size = 64'000'000;

	const auto memory = std::malloc(size);

	auto suballocator = lh::worst_fit_suballocator {memory, {0, size}};

	using allocator_type = pool_allocator<int>;
	using list_alloc_type = pool_allocator<double>;
	using deque_alloc_type = pool_allocator<char>;

	std::vector<int, allocator_type>* vector = new std::vector<int, allocator_type> {{0}, {suballocator}};
	std::vector<int, allocator_type>* vector2 = new std::vector<int, allocator_type> {{0}, {suballocator}};
	std::list<double, list_alloc_type>* list = new std::list<double, list_alloc_type> {suballocator};
	std::deque<char, deque_alloc_type>* deque = new std::deque<char, deque_alloc_type> {suballocator};

	for (size_t i = 0; i < 1000; i++)
	{
		vector->push_back(i);
		vector2->push_back(i * 2);
		list->push_back(std::numbers::pi * i);
		deque->push_back(i % 255);
		if (i == 999)
		{
			std::cout << "used mem: " << suballocator.used_memory_bytes() << '\n';
			std::cout << "free mem: " << suballocator.free_memory_bytes() << '\n';
			std::cout << "free mem %: " << suballocator.free_memory_ratio() << '\n';
		}
	}

	delete vector;
	delete vector2;
	delete list;
	delete deque;

	// auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	// engine.run();
}
