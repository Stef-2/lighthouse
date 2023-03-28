#pragma once

namespace lh
{
	// utility class to be inherited by any other that wishes to be static
	class static_t
	{
	public:
		static_t() = delete;
		static_t(const static_t&) = delete;
		static_t& operator=(const static_t&) = delete;

		vk::raii::Buffer b;
	};
}
