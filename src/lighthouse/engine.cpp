#include "engine.hpp"

lh::engine::engine(std::unique_ptr<window> window, const engine_version& engine_version,
                   const vulkan_version& vulkan_version)
    : m_window(std::move(window)), m_version(engine_version), m_renderer(*m_window, engine_version, vulkan_version)
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
    input::key_binding::bind({vkfw::Key::Escape},
                             [this]()
                             {
                                 m_window->vkfw_window().destroy();
                                 std::exit(0);
                             });

    input::key_binding::bind({vkfw::Key::F1},
                             [this]() { output::dump_logs(std::cout); });
}

auto lh::engine::poll_events() -> void
{
    vkfw::pollEvents();
}

auto lh::engine::terminate() -> void
{
    vkfw::terminate();
}

auto lh::engine::get_window() -> const window&
{
    return *m_window;
}

auto lh::engine::get_version() -> engine_version&
{
    return m_version;
}
