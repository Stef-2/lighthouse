module;

#if INTELLISENSE
#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>
#include <algorithm>
#include <ranges>
#endif

export module memory_suballocator;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	// manages memory block suballocations over a preallocated memory range
	// keeps track of taken and unused memory blocks
	// owns no memory itself
	class memory_suballocator
	{
	public:
		using initial_block_count_t = std::uint32_t;

		struct memory_block
		{
			void* m_memory_ptr;
			std::size_t m_size;

			auto operator>(const memory_block& other) const { return m_size > other.m_size; }
		};

		memory_suballocator(const memory_block& initial_memory, initial_block_count_t initial_block_count = 10);

		[[nodiscard]] auto request_and_commit_suballocation(const std::size_t) -> std::optional<void*>;
		auto free_suballocation(const memory_block&) -> void;

	private:
		auto sort_free_memory_blocks() -> void;

		// vector of free memory blocks, sorted by size in descending order
		std::vector<memory_block> m_free_memory_blocks;
	};
}