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

	pool_allocator(lh::first_fit_suballocator& memory_suballocator) : m_memory_suballocator {memory_suballocator} {}

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
		std::cout << "requesting: " << element_count * sizeof T << " bytes for this crap\n";
		return static_cast<T*>(*suballocation);
	}

	void deallocate(T* const pointer, size_t element_count) noexcept
	{
		std::cout << "freeing up: " << element_count * sizeof T << " bytes for this crap\n";
		m_memory_suballocator.free_suballocation({reinterpret_cast<std::size_t>(pointer), element_count * sizeof T});
	}

private:
	lh::first_fit_suballocator& m_memory_suballocator;
};

auto main() -> int
{
	const auto size = 64'000'000;

	const auto memory = std::malloc(size);

	auto suballocator = lh::first_fit_suballocator {memory, {0, size}};

	using allocator_type = pool_allocator<int>;

	std::vector<int, allocator_type>* vector = new std::vector<int, allocator_type> {{0}, {suballocator}};

	for (size_t i = 0; i < 1000; i++)
	{
		vector->push_back(i);

		if (i == 999)
		{
			std::cout << "used mem: " << suballocator.used_memory_bytes() << '\n';
			std::cout << "free mem: " << suballocator.free_memory_bytes() << '\n';
			std::cout << "free mem %: " << suballocator.free_memory_ratio() << '\n';
		}
	}

	delete vector;

	// auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	// engine.run();
}
