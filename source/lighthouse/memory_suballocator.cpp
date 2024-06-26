module;

module memory_suballocator;

namespace lh
{
	template <allocation_strategy A>
	memory_suballocator<A>::memory_suballocator(void* memory_ptr,
												const memory_block& initial_memory,
												initial_free_block_count_t initial_free_block_count)
		: m_ptr {static_cast<std::byte*>(memory_ptr) + initial_memory.m_offset},
		  m_initial_memory_block {initial_memory},
		  m_free_memory_blocks {initial_memory}
	{
		if (initial_free_block_count > 0) m_free_memory_blocks.reserve(initial_free_block_count);
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::request_and_commit_suballocation(const std::size_t size)
		-> const std::expected<void*, std::nullptr_t>
	{
		auto result = std::expected<void*, std::nullptr_t> {nullptr};

		// iterate over free memory blocks and attempt to find a free memory block large enough to claim
		// prefer to suballocate from smaller blocks, so iterate in reverse direction
		for (auto& free_memory : /*std::ranges::reverse_view(*/ m_free_memory_blocks /*)*/)
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

	template <allocation_strategy A>
	auto memory_suballocator<A>::free_suballocation(const memory_block& memory_block) -> void
	{
		// calculate the actual offset from the one provided by the pointer
		const auto offset = memory_block.m_offset - reinterpret_cast<std::size_t>(m_ptr);

		// reclaim memory by inserting a free block into the vector
		// sort it so its possible o merge adjecent free blocks
		// erase empty ones
		m_free_memory_blocks.emplace_back(offset, memory_block.m_size);
		sort_free_memory_blocks();
		merge_adjecent_free_memory_blocks();
		erase_empty_free_memory_blocks();
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::pointer() const -> const void*
	{
		return m_ptr;
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::address() const -> const std::uintptr_t
	{
		return reinterpret_cast<std::uintptr_t>(m_ptr);
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::used_memory_bytes() const -> const std::size_t
	{
		return m_initial_memory_block.m_size - free_memory_bytes();
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::free_memory_bytes() const -> const std::size_t
	{
		auto result = std::size_t {};

		for (const auto& free_memory_block : m_free_memory_blocks)
			result += free_memory_block.m_size;

		return result;
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::free_memory_ratio() const -> const float01_t
	{
		const auto free_memory = static_cast<float01_t>(this->free_memory_bytes());
		const auto total_memory = static_cast<float01_t>(m_initial_memory_block.m_size);

		return free_memory / total_memory;
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::erase_empty_free_memory_blocks() -> void
	{
		const auto empty_memory_blocks = std::ranges::remove_if(m_free_memory_blocks, [](const auto& element) {
			return element.m_size == 0;
		});

		m_free_memory_blocks.erase(empty_memory_blocks.begin(), empty_memory_blocks.end());
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::merge_adjecent_free_memory_blocks() -> void
	{
		for (auto i = std::size_t {1}; i < m_free_memory_blocks.size(); i++)
		{
			auto& previous = m_free_memory_blocks[i - 1];
			auto& current = m_free_memory_blocks[i];

			const auto memory_boundaries_align = previous.m_offset + previous.m_size == current.m_offset;

			if (memory_boundaries_align and previous.m_size and current.m_size)
			{
				current.m_offset = previous.m_offset;
				current.m_size += previous.m_size;
				previous.m_size = 0;
			}
		}
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::sort_free_memory_blocks() -> void
	{
		std::ranges::sort(m_free_memory_blocks, &comparison_fn);
	}
}
