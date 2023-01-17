#pragma once

#include "windows.h"

//#include <stdfloat>

namespace lh
{
	class memory
	{
	public:
		static auto get_available_memory() -> double;
	};
}