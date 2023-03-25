#pragma once

#include "lighthouse/renderer.hpp"
#include "lighthouse/version.hpp";
#include "lighthouse/window.hpp"

namespace lh
{
	// provides initialization and main loop facilities
	class engine
	{
	public:
		engine(std::unique_ptr<window>, const renderer::create_info& = {});

		~engine();

		// main engine loop
		auto run() -> void;

		auto window() -> const window&;
		auto version() -> version&;

	private:
		auto initialize() -> void;
		auto poll_events() -> void;
		auto terminate() -> void;

		std::unique_ptr<lh::window> m_window;
		renderer m_renderer;

		lh::version m_version;
	};
}
