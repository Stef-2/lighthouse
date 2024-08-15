module;

export module memory_block;

import std;

export namespace lh
{
	struct memory_block
	{
		std::size_t m_offset;
		std::size_t m_size;
	};
}