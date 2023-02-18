#pragma once

#include "input.hpp"
#include "renderer.hpp"
#include "version.hpp"
#include "window.hpp"

namespace lh
{
	// static engine system
	// provides initialization and main loop facilities
	class engine
	{
	public:
		engine(std::unique_ptr<window>, const renderer::create_info& = renderer::m_defaults);

		~engine();

		// main engine loop
		auto run() -> void;

		auto get_window() -> const window&;
		auto get_version() -> version&;

	private:
		auto initialize() -> void;
		auto poll_events() -> void;
		auto terminate() -> void;

		std::unique_ptr<window> m_window;
		renderer m_renderer;

		version m_version;
	};
}
