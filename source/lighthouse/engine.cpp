module;

#include <iostream>

#if INTELLISENSE
#include "lighthouse/engine.ixx"
#else
module engine;
#endif

#if INTELLISENSE
#include "lighthouse/output.ixx"
#include "lighthouse/input.ixx"
#else
import output;
import input;
#endif

namespace lh
{

	engine::engine(std::unique_ptr<lh::window> window, const create_info& engine_create_info)
		: m_window(std::move(window)),
		  m_version(engine_create_info.m_engine_version),
		  m_renderer(std::make_unique<renderer>(*m_window,
												renderer::create_info {engine_create_info.m_engine_version,
																	   engine_create_info.m_renderer_version}))
	{
		engine::initialize();
		input::initialize(*m_window);
		output::initialize();
	}

	engine::~engine()
	{
		terminate();
	}

	auto engine::run() -> void
	{
		while (!m_window->vkfw_window().shouldClose().value)
		{
			m_renderer->render();
			poll_events();
		}
	}

	auto engine::initialize() -> void
	{
		input::key_binding::bind({vkfw::Key::Escape}, [this]() {
			m_window->vkfw_window().destroy();
			std::exit(0);
		});

		input::key_binding::bind({vkfw::Key::F1}, [this]() { output::dump_logs(std::cout); });
	}

	auto engine::poll_events() -> void
	{
		vkfw::pollEvents();
	}

	auto engine::terminate() -> void
	{
		vkfw::terminate();
	}

	auto engine::window() -> const lh::window&
	{
		return *m_window;
	}

	auto engine::version() -> lh::version&
	{
		return m_version;
	}
}
