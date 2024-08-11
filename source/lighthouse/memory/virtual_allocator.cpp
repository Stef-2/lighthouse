module;

module virtual_allocator;

import output;

namespace lh
{
	virtual_allocator::virtual_allocator(const std::size_t size, create_info create_info)
		: m_virtual_block {vma::createVirtualBlock({size, create_info.m_create_flags})}, m_virtual_allocations {}
	{}

	virtual_allocator::virtual_allocator(virtual_allocator&& other)
		: m_virtual_block {std::exchange(other.m_virtual_block, {})}
	{}

	auto virtual_allocator::operator=(virtual_allocator&& other) -> virtual_allocator&
	{
		m_virtual_block = std::exchange(other.m_virtual_block, {});

		return *this;
	}

	virtual_allocator::~virtual_allocator()
	{
		m_virtual_block.destroy();
	}

	auto virtual_allocator::request_and_commit_suballocation(
		const std::size_t size, const allocation_info& allocation_info) -> const memory_offset_t
	{
		const auto create_info = vma::VirtualAllocationCreateInfo {size,
																   allocation_info.m_alignment,
																   allocation_info.m_allocation_flags};
		auto offset = std::numeric_limits<memory_offset_t>::max();
		auto allocation = vma::VirtualAllocation {};

		std::ignore = m_virtual_block.virtualAllocate(&create_info, &allocation, &offset);

		m_virtual_allocations.emplace(offset, allocation);

		return offset;
	}
	auto virtual_allocator::free_suballocation(const memory_offset_t offset) -> void
	{
		m_virtual_block.virtualFree(m_virtual_allocations[offset]);

		m_virtual_allocations.erase(offset);
	}
}
