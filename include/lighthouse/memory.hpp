#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef linux
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

#include "system.hpp"

#include <cstddef>
#include <iostream>
// #include <stdfloat>
namespace lh
{

  namespace memory
  {
	using memory_size_t = uint64_t;
	using decimal_t = double;
	using normalized_decimal_t = double;
	using vk_memory_type = uint32_t;

	static inline constexpr auto order_multiplier = 1024u;

	struct memory_unit_t
	{
	  memory_unit_t(memory_size_t value) : m_size(value) {};
	  memory_size_t m_size;
	  operator memory_size_t() { return m_size; }
	};

	struct bytes : public memory_unit_t
	{
	  using memory_unit_t::memory_unit_t;
	};

	struct kilobytes : public memory_unit_t
	{
	  using memory_unit_t::memory_unit_t;
	};

	struct megabytes : public memory_unit_t
	{
	  using memory_unit_t::memory_unit_t;
	};

	struct gigabytes : public memory_unit_t
	{
	  using memory_unit_t::memory_unit_t;
	};

	struct terabytes : public memory_unit_t
	{
	  using memory_unit_t::memory_unit_t;
	};

	enum class memory_sizes : memory_size_t
	{
	  byte = sizeof(std::byte),
	  kilobyte = byte * order_multiplier,
	  megabyte = kilobyte * order_multiplier,
	  gigabyte = megabyte * order_multiplier,
	  terabyte = gigabyte * order_multiplier
	};

	template <typename T>
	concept memory_unit = std::same_as<T, memory::bytes> || std::same_as<T, memory::kilobytes> ||
						  std::same_as<T, memory::megabytes> || std::same_as<T, memory::gigabytes> ||
						  std::same_as<T, memory::terabytes>;

	// ram memory info
	template <memory_unit unit> struct memory_info
	{
	  unit m_total {0};
	  unit m_available {0};
	  unit m_used {0};

	  normalized_decimal_t m_used_percentage {};
	};

	// gpu memory info
	struct physical_device_memory_info
	{
	  vk::DeviceSize m_device_total {};
	  vk::DeviceSize m_device_available {};
	  vk::DeviceSize m_device_used {};
	  normalized_decimal_t m_device_used_percentage {};

	  vk::DeviceSize m_shared_total {};
	  vk::DeviceSize m_shared_available {};
	  vk::DeviceSize m_shared_used {};
	  normalized_decimal_t m_shared_used_percentage {};
	};

	template <memory_unit unit = gigabytes> auto system_memory()
	{
	  constexpr auto unit_divisor = memory_size_t {
		std::same_as<unit, bytes>		? std::to_underlying(memory_sizes::byte)
		: std::same_as<unit, kilobytes> ? std::to_underlying(memory_sizes::kilobyte)
		: std::same_as<unit, megabytes> ? std::to_underlying(memory_sizes::megabyte)
		: std::same_as<unit, gigabytes> ? std::to_underlying(memory_sizes::gigabyte)
										: std::to_underlying(memory_sizes::terabyte)};

	  auto memory = memory_info<unit> {};

	  if constexpr (system::type == system::type::windows)
	  {
		auto status = MEMORYSTATUSEX {};
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);

		memory = {status.ullTotalPhys / unit_divisor,
				  status.ullAvailPhys / unit_divisor,
				  (status.ullTotalPhys - status.ullAvailPhys) / unit_divisor,
				  1.0 - static_cast<decimal_t>(status.ullAvailPhys) / static_cast<decimal_t>(status.ullTotalPhys)};
	  }
	  else if constexpr (system::type == system::type::linux)
	  {
		std::abort();
		// TODO
		/*
		struct sysinfo memInfo;

		sysinfo(&memInfo);
		long long totalVirtualMem = memInfo;*/
	  }

	  return memory;
	}

	auto physical_device_memory(const vk::raii::PhysicalDevice&) -> physical_device_memory_info;
  }

  // memory size literals
  constexpr auto operator"" _b(memory::memory_size_t value) -> memory::memory_size_t
  {
	return value * std::to_underlying(lh::memory::memory_sizes::byte);
  };
  constexpr auto operator"" _kb(memory::memory_size_t value) -> memory::memory_size_t
  {
	return value * std::to_underlying(lh::memory::memory_sizes::kilobyte);
  };
  constexpr auto operator"" _mb(memory::memory_size_t value) -> memory::memory_size_t
  {
	return value * std::to_underlying(lh::memory::memory_sizes::megabyte);
  };
  constexpr auto operator"" _gb(memory::memory_size_t value) -> memory::memory_size_t
  {
	return value * std::to_underlying(lh::memory::memory_sizes::gigabyte);
  };
  constexpr auto operator"" _tb(memory::memory_size_t value) -> memory::memory_size_t
  {
	return value * std::to_underlying(lh::memory::memory_sizes::terabyte);
  };
}
