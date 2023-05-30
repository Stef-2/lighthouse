#pragma once

template <typename T>
	requires std::integral<T>
auto aligned_size(T value, T alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}
