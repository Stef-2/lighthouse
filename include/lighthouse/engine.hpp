#pragma once

#include "static.hpp"
#include "window.hpp"
#include "input.hpp"

namespace lh
{
	class engine : static_t
	{
	public:
		static auto initialize(window&) -> void;
		static auto run() -> void;

	private:
		static auto bind_keyboard_input() -> void;
		static auto bind_mouse_input() -> void;

		static inline window* m_window = {};
	};
}