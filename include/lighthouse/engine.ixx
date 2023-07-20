module;
#pragma once

export module engine;

#if INTELLISENSE
#include "lighthouse/window.ixx"
#include "lighthouse/version.ixx"
#include "lighthouse/input.ixx"
#include "lighthouse/renderer/renderer.ixx"
#else
import window;
import version;
import input;
import renderer;
#endif

export namespace lh
{
	// provides initialization and main loop facilities
	class engine
	{
	public:
		struct create_info
		{
			lh::version m_engine_version {0, 1, 6};
			lh::version m_renderer_version {1, 3, 250};
		};

		engine(std::unique_ptr<window>, const create_info& = {});

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
