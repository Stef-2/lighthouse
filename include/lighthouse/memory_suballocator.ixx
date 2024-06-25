module;

export module memory_suballocator;

import std;

import data_type;

export namespace lh
{
	// manages memory block suballocations over a preallocated memory range
	// keeps track of taken and unused memory blocks
	// owns no memory itself
	class memory_suballocator
	{
	public:
		using suballocation_count_t = std::uint32_t;
		using initial_free_block_count_t = std::uint32_t;

		struct memory_block
		{
			std::size_t m_offset;
			std::size_t m_size;

			//auto offset()
			auto operator>(const memory_block& other) const { return m_size > other.m_size; }
		};

		memory_suballocator(void* memory_ptr,
							const memory_block& initial_memory,
							initial_free_block_count_t initial_block_count = 10);

		[[nodiscard]] auto request_and_commit_suballocation(const std::size_t) -> const std::expected<void*, std::nullptr_t>;
		auto free_suballocation(const memory_block&) -> void;

		auto pointer() const -> const void*;
		auto address() const -> const std::uintptr_t;

		auto used_memory() const -> const std::size_t;
		auto free_memory() const -> const std::size_t;
		auto free_memory_ratio() const -> const float01_t;

	private:
		auto erase_empty_free_memory_blocks() -> void;
		auto sort_free_memory_blocks() -> void;

		void* m_ptr;
		memory_block m_initial_memory_block;

		// vector of free memory blocks, sorted by size in descending order
		std::vector<memory_block> m_free_memory_blocks;
	};
}