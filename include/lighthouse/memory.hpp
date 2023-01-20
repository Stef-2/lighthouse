#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef linux
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "system.hpp"

#include <iostream>

namespace lh
{
    auto operator"" _b(uint64_t value) -> uint64_t;
    auto operator"" _kb(uint64_t value) -> uint64_t;
    auto operator"" _mb(uint64_t value) -> uint64_t;
    auto operator"" _gb(uint64_t value) -> uint64_t;

    namespace memory
    {
        using memory_t = uint64_t;
        using decimal_representation = double;
        using normalized_decimal = double;

        inline constexpr auto order_multiplier = 1024;

        struct bytes
        {
            memory_t m_bytes;
            operator memory_t()
            {
                return m_bytes;
            };
        };

        struct kilobytes
        {
            memory_t m_kilobytes;
            operator memory_t()
            {
                return m_kilobytes;
            };
        };

        struct megabytes
        {
            memory_t m_megabytes;
            operator memory_t()
            {
                return m_megabytes;
            };
        };

        struct gigabytes
        {
            memory_t m_gigabytes;
            operator memory_t()
            {
                return m_gigabytes;
            };
        };

        struct memory_info
        {
            bytes m_total_memory;
            bytes m_available_memory;
            bytes m_used_memory;

            normalized_decimal m_used_memory_percentage;
        };

        template <typename T>
        concept memory_unit = std::same_as<T, memory::bytes> || std::same_as<T, memory::kilobytes> ||
                              std::same_as<T, memory::megabytes> || std::same_as<T, memory::gigabytes>;

        enum class memory_unit
        {
            byte = sizeof(std::byte),
            kilobyte = byte * order_multiplier,
            megabyte = kilobyte * order_multiplier,
            gigabyte = megabyte * order_multiplier,
        };

        template <memory_unit U = gigabytes> auto system_memory() -> memory_info
        {
            constexpr auto unit_multiplier =
                memory_t {std::same_as<U, bytes>       ? std::to_underlying(memory_unit::byte)
                          : std::same_as<U, kilobytes> ? std::to_underlying(memory_unit::kilobyte)
                          : std::same_as<U, megabytes> ? std::to_underlying(memory_unit::megabyte)
                                                       : std::to_underlying(memory_unit::gigabyte)};
            auto memory = memory_info {};

            if constexpr (system::type == system::type::windows)
            {
                MEMORYSTATUSEX status {};
                status.dwLength = sizeof(status);
                GlobalMemoryStatusEx(&status);

                memory = {status.ullTotalPhys / unit_multiplier, status.ullAvailPhys / unit_multiplier,
                          (status.ullTotalPhys - status.ullAvailPhys) / unit_multiplier,
                          1.0 - static_cast<decimal_representation>(status.ullAvailPhys) /
                                    static_cast<decimal_representation>(status.ullTotalPhys)};
            }
            else if constexpr (system::type == system::type::linux)
            {
                // TODO
                struct sysinfo memInfo;

                sysinfo(&memInfo);
                long long totalVirtualMem = memInfo;
            }

            return memory;
        }
    }
}
