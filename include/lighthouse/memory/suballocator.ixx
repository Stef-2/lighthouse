module;

export module memory_suballocator;

import data_type;
import allocation_strategy;
import lighthouse_utility;

import std;

export namespace lh
{
	// manages memory block suballocations over a preallocated memory range
	// keeps track of used and free memory blocks
	// owns no memory itself
	template <allocation_strategy A/* = allocation_strategy::default_strategy*/>
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
							const initial_free_block_count_t initial_block_count = 32);

		memory_suballocator(const memory_suballocator&) = delete;
		auto operator=(const memory_suballocator&) -> memory_suballocator& = delete;

		[[nodiscard]] auto request_and_commit_suballocation(const std::size_t) -> void*;
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
	auto memory_suballocator<A>::request_and_commit_suballocation(const std::size_t size) -> void*
	{
		auto result = static_cast<void*>(nullptr);
		auto iterator = m_free_memory_blocks.end();

		// iterate over free memory blocks and attempt to find a free memory block large enough to claim
		// collect the resulting iterator, if found
		for (auto it = m_free_memory_blocks.begin(); it != m_free_memory_blocks.end(); ++it)
			if (size <= it->m_size)
			{
				result = static_cast<std::byte*>(m_ptr) + it->m_offset;
				iterator = it;
				it->m_offset += size;
				it->m_size -= size;

				break;
			}
		
		// if a valid iterator was found
		if (iterator != m_free_memory_blocks.end()) [[likely]]
			// if the underlying free memory block was taken in its entirety, erase it
			if (iterator->m_size == 0)
				m_free_memory_blocks.erase(iterator);
			else
				// memory blocks are sorted according to their offset
				std::ranges::sort(iterator, m_free_memory_blocks.end(), [this](const auto& x, const auto& y) {
					return x.m_offset < y.m_offset; // return comparison_fn(x, y);
				});

		return result;
	}

	template <allocation_strategy A>
	auto memory_suballocator<A>::free_suballocation(const memory_block& memory_block) -> void
	{
		// calculate the actual offset from the one provided by the pointer
		const auto offset = memory_block.m_offset - reinterpret_cast<std::size_t>(m_ptr);
		/*
		auto found = std::upper_bound(m_free_memory_blocks.begin(),
											m_free_memory_blocks.end(), offset,
											[](std::size_t x, const auto& y) { return x < y.m_offset;
											});
											*/
		/*
		auto found = std::ranges::find_if(std::ranges::reverse_view {m_free_memory_blocks},
										  [offset](const auto& x) {
			return offset < x.m_offset;
		}).base();*/

		//auto found = m_free_memory_blocks.rbegin();

		std::size_t i;

		for (i = 0; i < m_free_memory_blocks.size() - 1; i++/*i = std::size_t {m_free_memory_blocks.size() - 1}; i >= 0; --i*/)
		{
			//if (offset < m_free_memory_blocks[i].m_offset)
			{
				if (offset + memory_block.m_size == m_free_memory_blocks[i].m_offset)
				{
					m_free_memory_blocks[i].m_offset -= memory_block.m_size;
					m_free_memory_blocks[i].m_size += memory_block.m_size;
					//std::cout << "merging left\n";
					return;
				}
				else if (offset == m_free_memory_blocks[i].m_offset + m_free_memory_blocks[i].m_size)
				{
					m_free_memory_blocks[i].m_size += memory_block.m_size;
					//std::cout << "merging right\n";
					return;
				}
			}
		}

		const auto it = i == 0 ? m_free_memory_blocks.end() - 1 : m_free_memory_blocks.begin() + i;
		m_free_memory_blocks.emplace(it, memory_suballocator<A>::memory_block {offset, memory_block.m_size});
		//found = found.base();
		/*
		if (found != m_free_memory_blocks.rend())
		{
			// merge left
			if (offset + memory_block.m_size == found->m_offset)
			{
				found->m_offset -= memory_block.m_size;
				found->m_size += memory_block.m_size;
				std::cout << "merging left\n";
				return;
			}
		}
		else
		{
			if (found != m_free_memory_blocks.rbegin())
			{
				found++;
				// merge left
				if (found->m_offset + found->m_size == offset)
				{
					found->m_size += memory_block.m_size;
					std::cout << "merging right\n";
					return;
				}
				found--;
			}

		}*/

		// reclaim memory by inserting a free block into the vector
		// sort it so its possible to merge adjecent free blocks
		// erase empty ones
		/*
		m_free_memory_blocks.emplace(found != m_free_memory_blocks.rend()
										 ? found.base()
										 : m_free_memory_blocks.end() - 1,
									 memory_suballocator<A>::memory_block {offset, memory_block.m_size});*/
		/*
		sort_free_memory_blocks();
		merge_adjecent_free_memory_blocks();
		erase_empty_free_memory_blocks();*/
	}

	void fuck()
	{
		first_fit_suballocator ffs {nullptr, {0, 10}};
		auto wtf = ffs.request_and_commit_suballocation(32);
		ffs.free_suballocation({size_t(wtf), 32});
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