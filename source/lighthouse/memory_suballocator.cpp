module;

module memory_suballocator;

namespace lh
{
	memory_suballocator::memory_suballocator(void* memory_ptr,
											 const memory_block& initial_memory,
											 initial_free_block_count_t initial_free_block_count)
		: m_ptr {static_cast<std::byte*>(memory_ptr) + initial_memory.m_offset},
		  m_initial_memory_block {initial_memory},
		  m_free_memory_blocks {initial_memory}
	{
		if (initial_free_block_count > 0) m_free_memory_blocks.reserve(initial_free_block_count);
	}

	auto memory_suballocator::request_and_commit_suballocation(const std::size_t size)
		-> const std::expected<void*, std::nullptr_t>
	{
		auto result = std::expected<void*, std::nullptr_t> {nullptr};

		// iterate over free memory blocks and attempt to find a free memory block large enough to claim
		// prefer to suballocate from smaller blocks, so iterate in reverse direction
		for (auto& free_memory : std::ranges::reverse_view(m_free_memory_blocks))
			if (size <= free_memory.m_size)
			{
				result = static_cast<std::byte*>(m_ptr) + free_memory.m_offset;
				free_memory.m_offset += size;
				free_memory.m_size -= size;

				break;
			}

		// if a free memory block was found and commited, sort free memory blocks
		if (result)
		{
			erase_empty_free_memory_blocks();
			sort_free_memory_blocks();
		}

		return result;
	}

	auto memory_suballocator::free_suballocation(const memory_block& memory_block) -> void
	{
		// calculate the actual offset from the one provided by the pointer
		const auto offset = memory_block.m_offset - reinterpret_cast<std::size_t>(m_ptr);

		// attempt to find a free adjecent memory block and merge with it
		// prefer to merge with larger blocks, so iterate in forward direction
		for (auto& free_memory : m_free_memory_blocks)
		{
			const auto space_for_rhs_merge = free_memory.m_offset + free_memory.m_size + memory_block.m_size <=
											 m_initial_memory_block.m_offset + m_initial_memory_block.m_size;
			const auto alignment_for_rhs_merge = free_memory.m_offset + free_memory.m_size == offset;

			const auto space_for_lhs_merge = free_memory.m_offset >= memory_block.m_size;
			const auto alignment_for_lhs_merge = free_memory.m_offset == offset + memory_block.m_size;

			if (space_for_rhs_merge and alignment_for_rhs_merge)
			{
				free_memory.m_size += memory_block.m_size;
			} else if (space_for_lhs_merge and alignment_for_lhs_merge)
			{
				free_memory.m_offset -= memory_block.m_size;
				free_memory.m_size += memory_block.m_size;
			}

			// if (free_memory.m_size == 0) __debugbreak();

			if ((space_for_rhs_merge and alignment_for_rhs_merge) or (space_for_lhs_merge and alignment_for_lhs_merge))
			{
				erase_empty_free_memory_blocks();
				sort_free_memory_blocks();
				return;
			}
		}

		// there were no free adjecent memory_block blocks to merge with, create a new one
		m_free_memory_blocks.emplace_back(offset, memory_block.m_size);
		sort_free_memory_blocks();
	}

	auto memory_suballocator::pointer() const -> const void*
	{
		return m_ptr;
	}

	auto memory_suballocator::address() const -> const std::uintptr_t
	{
		return reinterpret_cast<std::uintptr_t>(m_ptr);
	}

	auto memory_suballocator::used_memory() const -> const std::size_t
	{
		return m_initial_memory_block.m_size - free_memory();
	}

	auto memory_suballocator::free_memory() const -> const std::size_t
	{
		auto result = std::size_t {};

		for (const auto& free_memory_block : m_free_memory_blocks)
			result += free_memory_block.m_size;

		return result;
	}

	auto memory_suballocator::free_memory_ratio() const -> const float01_t
	{
		const auto free_memory = static_cast<float01_t>(this->free_memory());
		const auto total_memory = static_cast<float01_t>(m_initial_memory_block.m_size);

		return free_memory / total_memory;
	}

	auto memory_suballocator::erase_empty_free_memory_blocks() -> void
	{
		const auto empty_memory_blocks = std::ranges::remove_if(m_free_memory_blocks, [](const auto& element) {
			return element.m_size == 0;
		});

		m_free_memory_blocks.erase(empty_memory_blocks.begin(), empty_memory_blocks.end());
	}

	auto memory_suballocator::sort_free_memory_blocks() -> void
	{
		std::ranges::sort(m_free_memory_blocks, std::greater {});
	}
}
