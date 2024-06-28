module;

export module memory_pool;

import allocation_strategy;
import memory_suballocator;
import output;

import std;

export namespace lh
{
	// stateful allocator
	// claims memory from a memory suballocator
	template <typename T, lh::allocation_strategy A = lh::allocation_strategy::default_strategy>
	class pool_allocator
	{
	public:
		using value_type = T;
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;
		using is_always_equal = std::true_type;

		template <typename, lh::allocation_strategy>
		friend class pool_allocator;

		template <typename Y>
		struct rebind
		{
			using other = pool_allocator<Y, A>;
		};

		pool_allocator(lh::memory_suballocator<A>& memory_suballocator) : m_memory_suballocator {&memory_suballocator}
		{}
		template <class Y>
		pool_allocator(const pool_allocator<Y>& other) noexcept : m_memory_suballocator {other.m_memory_suballocator}
		{}

		auto select_on_container_copy_construction() const -> pool_allocator { return *this; }

		T* allocate(const size_t element_count) noexcept
		{
			const auto suballocation = m_memory_suballocator->request_and_commit_suballocation(element_count *
																							   sizeof T);
			return static_cast<T*>(suballocation);
		}

		void deallocate(const T* pointer, const size_t element_count) noexcept
		{
			m_memory_suballocator->free_suballocation(
				{reinterpret_cast<std::size_t>(pointer), element_count * sizeof T});
		}

	private:
		lh::memory_suballocator<A>* m_memory_suballocator;
	};

	// ==========================================================================

	// fixed size memory pool
	template <lh::allocation_strategy A = lh::allocation_strategy::default_strategy>
	class memory_pool
	{
	public:
		memory_pool(const std::size_t size) : m_memory {std::malloc(size)}, m_size {size}, m_suballocator {m_memory, {0, size}}
		{
			assert_allocation_success(size);
		}

		// disallow copy construction
		memory_pool(const memory_pool&) = delete;
		auto operator=(const memory_pool&) -> memory_pool& = delete;

		memory_pool(memory_pool&& other) noexcept
			: m_memory {std::exchange(other.m_memory, {})},
			  m_size {std::exchange(other.m_size, {})},
			  m_suballocator {std::exchange(other.m_suballocator, {})}
		{}
		auto operator=(memory_pool&& other) noexcept -> memory_pool&
		{
			m_memory = std::exchange(other.m_memory, {});
			m_size = std::exchange(other.m_size, {});
			m_suballocator = std::exchange(other.m_suballocator, {});

			return *this;
		}

		~memory_pool()
		{
			if (m_memory) std::free(m_memory);
		}

		auto size() const -> const std::size_t { return m_size; }
		auto data() -> void* { return m_memory; }
		auto resize(const std::size_t new_size)
		{
			if (new_size <= m_size)
			{
				lh::output::warning() << "resizing memory pool requires a size larger than: " << m_size;
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
			return pool_allocator<T, A> {m_suballocator};
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
}