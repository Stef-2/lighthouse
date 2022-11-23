#pragma once

#include "Static.hpp"
#include "Window.hpp"

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