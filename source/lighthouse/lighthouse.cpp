#include "vkfw/vkfw.hpp"
#include <span>
import engine;
import window;

#if not INTELLISENSE
import std.core;
#endif

template <typename T>
class empty_allocator
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using pointer = T*;

	pointer memory_ptr;
	std::size_t memory_size;

	empty_allocator(T* address, std::size_t size) noexcept : memory_ptr {address}, memory_size {size} {}

	empty_allocator(const empty_allocator& other) noexcept
		: memory_ptr {other.memory_ptr}, memory_size {other.memory_size}
	{
		std::cout << "copying\n";
	}

	template <typename U>
	empty_allocator(const empty_allocator<U>& other) noexcept
		: memory_ptr {(T*)other.memory_ptr}, memory_size {other.memory_size}
	{
		std::cout << "copying\n";
		//*this = std::move(other);
	}

	~empty_allocator() {}

	template <typename U>
	bool operator==(const empty_allocator<U>&) const noexcept
	{
		std::cout << "comparing\n";
		return true;
	}

	template <typename U>
	bool operator!=(const empty_allocator<U>&) const noexcept
	{
		std::cout << "cmp !=\n";
		return false;
	}

	pointer allocate(const std::size_t n) const
	{
		std::cout << "allocating\n";
		return memory_ptr;
	};
	void deallocate(T* const p, size_t) const noexcept {};
	size_type max_size() const { return memory_size; }
};

auto main() -> int
{
	// std::_Container_proxy b;
	int* b;
	std::vector<int> v = {4, 5, 6};
	auto p = v.data();
	empty_allocator<int> a = {p, 4 * sizeof(int)};
	std::vector<int, empty_allocator<int>> arr = {{1, 2, 3}, a};

	arr[0] = 9;
	arr.emplace_back(77);

	// std::span<int, 66> s();
	auto sp = std::span {v};
	b = v.data();

	auto spa = std::span<int>(b, 3);
	// std::allocator<int> a;
	// a.
	//  auto engine = lh::engine {std::make_unique<lh::window>(lh::window::create_info {.m_resolution {1280, 720}})};

	// engine.run();
}
