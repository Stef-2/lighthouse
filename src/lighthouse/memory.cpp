#include "memory.hpp"

auto lh::operator"" _b(uint64_t value) -> uint64_t
{
    return value;
}

auto lh::operator"" _kb(uint64_t value) -> uint64_t
{
    return value * std::to_underlying(lh::memory::memory_unit::kilobyte);
}

auto lh::operator"" _mb(uint64_t value) -> uint64_t
{
    return value * std::to_underlying(lh::memory::memory_unit::megabyte);
}

auto lh::operator"" _gb(uint64_t value) -> uint64_t
{
    return value * std::to_underlying(lh::memory::memory_unit::gigabyte);
}