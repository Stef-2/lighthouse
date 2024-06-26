module;

export module memory_suballocator;

import data_type;
import lighthouse_utility;

import std;

export namespace lh
{
	enum class allocation_strategy
	{
		first_fit,
		best_fit,
		worst_fit,
		default_strategy = first_fit
	};

	// manages memory block suballocations over a preallocated memory range
	// keeps track of used and free memory blocks
	// owns no memory itself
	template <allocation_strategy A = allocation_strategy::default_strategy>
	class memory_suballocator
	{
	public:
		using suballocation_count_t = std::uint32_t;
		using initial_free_block_count_t = std::uint32_t;

		struct memory_block
		{
			std::size_t m_offset;
			std::size_t m_size;
		};

		memory_suballocator(non_owning_ptr<void> memory_ptr,
							const memory_block& initial_memory,
							const initial_free_block_count_t initial_block_count = 10);

		memory_suballocator(const memory_suballocator&) = delete;
		auto operator=(const memory_suballocator&) -> memory_suballocator& = delete;
		//memory_suballocator(const memory_suballocator&&) = delete;

		[[nodiscard]] auto request_and_commit_suballocation(const std::size_t)
			-> const std::expected<void*, std::nullptr_t>;
		auto free_suballocation(const memory_block&) -> void;

		auto pointer() const -> const non_owning_ptr<void>;
		auto address() const -> const std::uintptr_t;

		auto used_memory_bytes() const -> const std::size_t;
		auto free_memory_bytes() const -> const std::size_t;
		auto free_memory_ratio() const -> const float01_t;

	private:
		// function used for free memory block sorting, depends on allocation strategy
		auto comparison_fn(const memory_block&, const memory_block&) const -> bool;

		auto erase_empty_free_memory_blocks() -> void;
		auto merge_adjecent_free_memory_blocks() -> void;
		auto sort_free_memory_blocks() -> void;

		non_owning_ptr<void> m_ptr;
		memory_block m_initial_memory_block;

		// vector of free memory blocks, sorted by offset in ascending order
		std::vector<memory_block> m_free_memory_blocks;
	};

	using first_fit_suballocator = memory_suballocator<allocation_strategy::first_fit>;
	using best_fit_suballocator = memory_suballocator<allocation_strategy::best_fit>;
	using worst_fit_suballocator = memory_suballocator<allocation_strategy::worst_fit>;

	// ============================================================================
	// template instantiations
	// ============================================================================

	template <allocation_strategy A>
	memory_suballocator<A>::memory_suballocator(non_owning_ptr<void> memory_ptr,
												const memory_block& initial_memory,
												const initial_free_block_count_t initial_free_block_count)
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
		for (auto& free_memory : m_free_memory_blocks)
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
		// sort it so its possible to merge adjecent free blocks
		// erase empty ones
		m_free_memory_blocks.emplace_back(offset, memory_block.m_size);
		sort_free_memory_blocks();
		merge_adjecent_free_memory_blocks();
		erase_empty_free_memory_blocks();
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::pointer() const -> const non_owning_ptr<void>
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
	auto memory_suballocator<A>::comparison_fn(const memory_block& x, const memory_block& y) const -> bool
	{
		if constexpr (A == allocation_strategy::first_fit) return x.m_offset < y.m_offset;

		if constexpr (A == allocation_strategy::best_fit)
		{
			if (x.m_offset < y.m_offset) return true;
			if (y.m_offset < x.m_offset) return false;
			if (x.m_size < y.m_size) return true;
			if (y.m_size < x.m_size) return false;
		}

		if constexpr (A == allocation_strategy::worst_fit)
		{
			if (x.m_offset < y.m_offset) return true;
			if (y.m_offset < x.m_offset) return false;
			if (x.m_size > y.m_size) return true;
			if (y.m_size > x.m_size) return false;
		}

		std::unreachable();
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
		std::ranges::sort(m_free_memory_blocks, [this](const auto& x, const auto& y) { return comparison_fn(x, y); });
	}
}