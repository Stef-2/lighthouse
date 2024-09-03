module;

#include "vkfw/vkfw.hpp"

module engine;

import file_system;
import output;
import input;
import time;

namespace lh
{
	engine::engine(std::unique_ptr<lh::window> window, const create_info& engine_create_info)
		: m_window {std::move(window)}, m_renderer {}, m_version(engine_create_info.m_engine_version)
	{
		file_system::initialize();
		input::initialize(*m_window);
		engine::initialize();
		output::initialize();

		m_renderer = std::make_unique<renderer>(*m_window,
												renderer::create_info {engine_create_info.m_engine_version,
																	   engine_create_info.m_renderer_version});
	}

	engine::~engine()
	{
		terminate();
	}

	auto engine::run() -> void
	{
		while (!m_window->vkfw_window().shouldClose().value)
		{
			poll_events();
			input::key_binding::execute_pressed_keys();
			m_renderer->render();
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
