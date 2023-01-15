#include "window.hpp"

lh::window::window(lh::window::window_resolution resolution, std::string_view name, bool fullscreen,
                   vkfw::WindowHints& hints)
    : m_title(name)
{
    initialize_vkfw();

    // hint that disables OpenGL context creation and enables Vulkan instead
    hints.clientAPI = vkfw::ClientAPI::None;

    // set the monitor depending on fullscreen mode
    m_monitor = fullscreen ? vkfw::getPrimaryMonitor().value : nullptr;

    auto result = vkfw::Result {};
    std::tie(result, m_window) =
        vkfw::createWindowUnique(resolution.width, resolution.height, defaultName, hints, m_monitor).asTuple();

    if (!vkfw::check(result))
    {
        output::fatal() << "Could not initialize VKFW / GLFW window";
    }
}

lh::window::window(const window&& other) noexcept : window(const_cast<window&&>(other))
{
}

auto lh::window::get_resolution() const -> window_resolution
{
    auto [width, height] {m_window.get().getFramebufferSize().value};

    return window_resolution {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

auto lh::window::get_aspect_ratio() const -> double
{
    auto&& [width, height] = m_window.get().getFramebufferSize().value;

    return static_cast<double>(width) / static_cast<double>(height);
}

auto lh::window::get_title() const -> std::string_view
{
    return m_title;
}

auto lh::window::set_title(std::string_view value) -> void
{
    m_title = value;
    m_window.get().setTitle(value);
}

auto lh::window::get_fullscreen() const -> bool
{
    return m_monitor;
}

auto lh::window::set_fullscreen(bool value) -> void
{
    if (get_fullscreen() == value)
        return;

    // set the monitor depending on fullscreen mode
    m_monitor = value ? vkfw::getPrimaryMonitor().value : nullptr;
}

auto lh::window::vkfw_window() const -> vkfw::Window&
{
    return m_window.get();
}

auto lh::window::vkfw_monitor() -> vkfw::Monitor&
{
    return m_monitor;
}

auto lh::window::initialize_vkfw() const -> void
{
    auto once = std::once_flag {};

    auto initialize = []()
    {
        auto success = vkfw::init();

        if (!vkfw::check(success))
        {
            lh::output::fatal() << "Could not initialize VKFW / GLFW.";
        }
    };

    std::call_once(once, initialize);
}
