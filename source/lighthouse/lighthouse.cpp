#include "vkfw/vkfw.hpp"

import engine;
import window;
import object_index;

#if not INTELLISENSE
import std.core;
#endif

import memory_suballocator;
import output;

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

	T* allocate(const size_t element_count) noexcept
	{
		const auto suballocation = m_memory_suballocator.request_and_commit_suballocation(element_count * sizeof T);
		return static_cast<T*>(*suballocation);
	}

	void deallocate(const T* pointer, const size_t element_count) noexcept
	{
		m_memory_suballocator.free_suballocation({reinterpret_cast<std::size_t>(pointer), element_count * sizeof T});
	}

private:
	lh::first_fit_suballocator& m_memory_suballocator;
};

// ==========================================================

template <lh::allocation_strategy A = lh::allocation_strategy::default_strategy>
class pool
{
public:
	pool(const std::size_t size) : m_memory {std::malloc(size)}, m_size {size}, m_suballocator {m_memory, {0, size}}
	{
		assert_allocation_success(size);
	}

	// disallow copy construction
	pool(const pool&) = delete;
	auto operator=(const pool&) -> pool& = delete;

	pool(pool&& other) noexcept
		: m_memory {std::exchange(other.m_memory, {})},
		  m_size {std::exchange(other.m_size, {})},
		  m_suballocator {std::exchange(other.m_suballocator, {})}
	{}
	auto operator=(pool&& other) noexcept -> pool&
	{
		m_memory = std::exchange(other.m_memory, {});
		m_size = std::exchange(other.m_size, {});
		m_suballocator = std::exchange(other.m_suballocator, {});

		return *this;
	}

	~pool()
	{
		if (m_memory) std::free(m_memory);
	}

	auto size() const -> const std::size_t { return m_size; }
	auto resize(const std::size_t new_size)
	{
		if (new_size <= m_size)
		{
			lh::output::warning() << "resizing a memory pool requires larger than previous size: " << m_size;
			return;
		}

		m_memory = std::realloc(new_size);
		assert_allocation_success(new_size);
		m_size = new_size;
		m_suballocator = {m_memory, {0, m_size}};
	}

	auto suballocator() -> lh::memory_suballocator<A>& { return m_suballocator; }

	template <typename T>
	auto allocator() -> pool_allocator<T>
	{
		return pool_allocator<T> {m_suballocator};
	}

	template <typename T, typename... Ts>
	auto vector(auto&&... ts)
	{
		return std::vector<T, pool_allocator<T>> {std::forward<Ts>(ts)..., {m_suballocator}};
	}

private:
	auto assert_allocation_success(const std::size_t size) const -> void
	{
		if (not m_memory) lh::output::error() << "could not allocate: " << size << " bytes for memory pooling";
	}

	void* m_memory;
	std::size_t m_size;
	lh::memory_suballocator<A> m_suballocator;
};

// =============================================================================

auto main() -> int
{
	const auto size = 64'000'000;

	// const auto memory = std::malloc(size);

	pool pool_(size);

	// auto& suballocator = pool.suballocator(); // lh::worst_fit_suballocator {memory, {0, size}};
	auto allocator = pool_.allocator<int>();
	auto i16_all = pool_.allocator<std::int16_t>();
	auto dbl_allocator = pool_.allocator<double>();
	auto chr_allocator = pool_.allocator<char>();

	using allocator_type = pool_allocator<int>;
	using list_alloc_type = pool_allocator<double>;
	using deque_alloc_type = pool_allocator<char>;

	using vec_t = std::vector<int, allocator_type>;

	vec_t* vector = new vec_t {{0}, allocator};
	std::vector<int, allocator_type>* vector2 = new std::vector<int, allocator_type> {{0}, allocator};
	std::list<double, list_alloc_type>* list = new std::list<double, list_alloc_type> {dbl_allocator};
	std::deque<char, deque_alloc_type>* deque = new std::deque<char, deque_alloc_type> {chr_allocator};

	auto test = new std::vector<std::int16_t, pool_allocator<std::int16_t>> {i16_all};

	*test = pool_.vector<std::int16_t>();

	for (size_t i = 0; i < 1000; i++)
	{
		vector->push_back(i);
		vector2->push_back(i * 2);
		list->push_back(std::numbers::pi * i);
		deque->push_back(i % 255);
		test->push_back(std::pow(i, 4));
		if (i == 999)
		{
			// std::cout << "used mem: " << suballocator.used_memory_bytes() << '\n';
			// std::cout << "free mem: " << suballocator.free_memory_bytes() << '\n';
			// std::cout << "free mem %: " << suballocator.free_memory_ratio() << '\n';
		}
	}

	delete vector;
	delete vector2;
	delete list;
	delete deque;
	delete test;

	// auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	// engine.run();
}
