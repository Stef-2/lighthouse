module;

module memory_suballocator;

namespace lh
{
	memory_suballocator::memory_suballocator(const memory_block& initial_memory,
											 initial_block_count_t initial_block_count)
		: m_free_memory_blocks {initial_memory}
	{
		m_free_memory_blocks.reserve(initial_block_count);
	}

	auto memory_suballocator::request_and_commit_suballocation(const std::size_t size) -> std::optional<void*>
	{
		auto result = std::optional<void*> {};

		// iterate over free memory blocks and attempt to find a free memory block large enough to claim
		// prefer to suballocate from smaller blocks, so iterate in reverse direction
		for (auto& free_memory : std::ranges::reverse_view(m_free_memory_blocks))
			if (size <= free_memory.m_size)
			{
				result = free_memory.m_memory_ptr;
				free_memory.m_memory_ptr = static_cast<std::byte*>(free_memory.m_memory_ptr) + size;
				break;
			}

		// if a free memory block was found and commited, sort free memory blocks
		if (result) sort_free_memory_blocks();

		return result;
	}

	auto memory_suballocator::free_suballocation(const memory_block& memory_block) -> void
	{
		// attempt to find a free adjecent memory block and merge with it
		// prefer to merge with larger blocks, so iterate in forward direction
		for (auto& free_memory : m_free_memory_blocks)
		{
			const auto merge_lhs = static_cast<std::byte*>(free_memory.m_memory_ptr) + free_memory.m_size ==
								   static_cast<std::byte*>(memory_block.m_memory_ptr);
			const auto merge_rhs = static_cast<std::byte*>(free_memory.m_memory_ptr) ==
								   static_cast<std::byte*>(memory_block.m_memory_ptr) + memory_block.m_size;

			if (merge_lhs)
				free_memory.m_size += memory_block.m_size;
			else if (merge_rhs)
				free_memory.m_memory_ptr = static_cast<std::byte*>(free_memory.m_memory_ptr) - memory_block.m_size;

			if (merge_lhs or merge_rhs)
			{
				sort_free_memory_blocks();
				return;
			}
		}

		// there were no free adjecent memory_block blocks to merge with, create a new one
		m_free_memory_blocks.emplace_back(memory_block);
		sort_free_memory_blocks();
	}

	auto memory_suballocator::sort_free_memory_blocks() -> void
	{
		std::ranges::sort(m_free_memory_blocks, std::greater {});
	}
}
