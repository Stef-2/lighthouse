module;

#if INTELLISENSE
	#include "vma/vk_mem_alloc.hpp"
#endif

export module virtual_allocator;

#if not INTELLISENSE
export import vk_mem_alloc_hpp;
#endif

import std;

export namespace lh
{
	class virtual_allocator
	{
	public:
		struct create_info
		{
			vma::VirtualBlockCreateFlagBits m_create_flags {};
		};

		struct allocation_info
		{
			std::size_t m_alignment {};
			vma::VirtualAllocationCreateFlags m_allocation_flags {};
		};

		virtual_allocator(const std::size_t, create_info = {});

		// disallow copy construction
		virtual_allocator(const virtual_allocator&) = delete;
		auto operator=(const virtual_allocator&) -> virtual_allocator& = delete;

		virtual_allocator(virtual_allocator&&);
		auto operator=(virtual_allocator&&) -> virtual_allocator&;

		~virtual_allocator();

		auto request_and_commit_suballocation(const std::size_t, const allocation_info& = {}) const -> const vma::VirtualAllocation;
		auto free_suballocation(const vma::VirtualAllocation) const -> void;
		
	private:
		vma::VirtualBlock m_virtual_block;
	};
}