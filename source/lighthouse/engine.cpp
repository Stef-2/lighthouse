#include "lighthouse/engine.hpp"
#include "lighthouse/window.hpp"
#include "lighthouse/version.hpp"
#include "lighthouse/input.hpp"

lh::engine::engine(std::unique_ptr<lh::window> window, const renderer::create_info& create_info)
	: m_window(std::move(window)), m_version(create_info.m_engine_version), m_renderer(*m_window, create_info)
{
	engine::initialize();
	input::initialize(*m_window);
	output::initialize();
}

lh::engine::~engine()
{
	terminate();
}

auto lh::engine::run() -> void
{
	while (!m_window->vkfw_window().shouldClose().value)
	{
		m_renderer.render();
		poll_events();
	}
}

auto lh::engine::initialize() -> void
{
	input::key_binding::bind({vkfw::Key::Escape}, [this]() {
		m_window->vkfw_window().destroy();
		std::exit(0);
	});

	input::key_binding::bind({vkfw::Key::F1}, [this]() { output::dump_logs(std::cout); });
}

auto lh::engine::poll_events() -> void
{
	vkfw::pollEvents();
}

auto lh::engine::terminate() -> void
{
	vkfw::terminate();
}

auto lh::engine::window() -> const lh::window&
{
	return *m_window;
}

auto lh::engine::version() -> lh::version&
{
	return m_version;
}