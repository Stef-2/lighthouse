#pragma once

#include "static.hpp"
#include "window.hpp"

namespace lh
{
	class engine : static_t
	{
	public:
		static auto initialize(const window&) -> void;
		static auto run() -> void;

	private:

	};
}