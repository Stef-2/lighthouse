#pragma once

#include "window.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include "version.hpp"

namespace lh
{
	// static engine system
	// provides initialization and main loop facilities
	class engine
	{
	public:
		engine(const window&,
			   const engine_version& = engine_version::m_default,
			   const vulkan_version& = vulkan_version::m_default);

		~engine();

		// main engine loop
		auto run() -> void;

		auto get_window() -> const window&;
		auto get_version() -> engine_version&;

	private:
		auto initialize() -> void;
		auto poll_events() -> void;
		auto terminate() -> void;

		window m_window;
		renderer m_renderer;

		engine_version m_version;
	};
}