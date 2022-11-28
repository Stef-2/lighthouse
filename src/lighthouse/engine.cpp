#include "engine.hpp"

auto lh::engine::initialize(window& window, const vulkan_version& version) -> void
{
    auto once = std::once_flag {};

    auto initialize = [&window, &version]() {
        m_window = &window;

        renderer::initialize(window, version);
        input::initialize(window);
        output::initialize();
    };

    std::call_once(once, initialize);
}

auto lh::engine::run() -> void
{
    while (!m_window->vkfw_window().shouldClose().value)
    {
        vkfw::pollEvents();
    }
}

auto lh::engine::get_window() -> window&
{
    return *m_window;
}

auto lh::engine::get_version() -> engine_version&
{
    return m_version;
}

lh::engine_version::operator std::string()
{
    return "engine version: " + m_major + ':' + m_minor + ':' + m_patch;
}

lh::engine_version::engine_version(version_t major, version_t minor, version_t patch)
    : m_major(major), m_minor(minor), m_patch(patch)
{
}

lh::engine_version::engine_version(packed_version_t version)
    : m_major(version >> 16 & 0xFF), m_minor(version >> 8 & 0xFF), m_patch(version & 0xFF)
{
}

lh::engine_version::operator packed_version_t()
{
    return m_major << 16 | m_minor << 8 | m_patch;
}
