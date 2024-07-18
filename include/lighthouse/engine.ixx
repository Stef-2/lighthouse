module;

export module engine;

import window;
import version;
import renderer;

import std;

export namespace lh
{
	// provides initialization and main loop facilities
	class engine
	{
	public:
		using delta_time_t = double;

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
		std::unique_ptr<renderer> m_renderer;

		lh::version m_version;
	};
}
