#include "vkfw/vkfw.hpp"

module engine;

import output;
import input;
import time;

namespace lh
{

	engine::delta_time_t engine::s_delta_time = vkfw::getTime().value;

	engine::engine(std::unique_ptr<lh::window> window, const create_info& engine_create_info)
		: m_window {std::move(window)}, m_renderer {}, m_version(engine_create_info.m_engine_version)
	{
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
			m_renderer->render();
			poll_events();

			s_delta_time = vkfw::getTime().value - s_delta_time;
		}
	}

	auto engine::initialize() -> void
	{
		input::keyboard::bind({vkfw::Key::Escape}, [this]() {
			m_window->vkfw_window().destroy();
			std::exit(0);
		});

		input::keyboard::bind({vkfw::Key::F1}, [this]() { output::dump_logs(std::cout); });
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

	auto engine::delta_time() -> const delta_time_t&
	{
		return s_delta_time;
	}
}
