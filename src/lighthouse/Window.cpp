#include "Window.hpp"

lh::Window::Window(lh::Window::WindowResolution resolution, std::string_view name, bool fullscreen,
                   vkfw::WindowHints& hints)
    : title(name)
{
    initialize_vkfw();

    // hint that disables OpenGL context creation and enables Vulkan instead
    hints.clientAPI = vkfw::ClientAPI::None;

    // set the monitor depending on fullscreen mode
    monitor = fullscreen ? vkfw::getPrimaryMonitor().value : nullptr;

    auto result = vkfw::Result {};
    std::tie(result, window) = vkfw::createWindowUnique(resolution.first, resolution.second, defaultName, hints, monitor).asTuple();
    
    if (!vkfw::check(result))
    {
        Output::error() << "Could not initialize VKFW / GLFW window";
        std::abort();
    }
}

lh::Window::Window()
    : lh::Window::Window(lh::Window::common_resolutions.at(lh::Window::CommonResolutions::Default_fullscreen))
{
}

lh::Window::~Window()
{
    vkfw::terminate();
}

auto lh::Window::get_aspect_ratio() const -> double
{
    auto&& [width, height] = window.get().getSize().value;

    return width / height;
}

auto lh::Window::get_title() const -> std::string_view
{
    return title;
}

auto lh::Window::set_title(std::string_view value) -> void
{
    title = value;
    window.get().setTitle(value);
}

auto lh::Window::get_fullscreen() const -> bool
{
    return monitor;
}

auto lh::Window::set_fullscreen(bool value) -> void
{
    if (get_fullscreen() == value)
        return;

    // set the monitor depending on fullscreen mode
    monitor = value ? vkfw::getPrimaryMonitor().value : nullptr;
}

auto lh::Window::vkfw_window() -> vkfw::Window&
{
    return window.get();
}

auto lh::Window::vkfw_monitor() -> vkfw::Monitor&
{
    return monitor;
}

auto lh::Window::initialize_vkfw() const -> void
{
    if (lh::Window::vkfwInitialzied)
        return;

    auto success = vkfw::init();

    if (!vkfw::check(success))
    {
        lh::Output::error() << "Could not initialize VKFW / GLFW.";
        std::abort();
    }

    lh::Window::vkfwInitialzied = true;
}
