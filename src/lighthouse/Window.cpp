#include "Window.hpp"

bool lh::Window::vkfwInitialzied = false;

lh::Window::Window(WindowDimension width, WindowDimension height, std::string_view name, bool fullscreen,
                   const std::vector<std::pair<WindowHint, WindowHintValue>>& hints)
    : width(width), height(height), name(name), fullscreen(fullscreen)
{
    InitializeGLFW();

    // parse and apply window creation hints
    for (auto& [hint, value] : hints)
        glfwWindowHint(std::to_underlying(hint), std::to_underlying(value));

    // hint that disables OpenGL context creation and enables vulkan instead
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwMonitor = glfwGetPrimaryMonitor();
    glfwVideoMode = *glfwGetVideoMode(glfwMonitor);

    GLFWmonitor* useFullscreen = fullscreen ? glfwMonitor : nullptr;
    glfwWindow = glfwCreateWindow(width, height, this->name.c_str(), useFullscreen, nullptr);
}

lh::Window::Window(lh::Window::WindowResolution resolution, std::string_view name, bool fullscreen,
                   const std::vector<std::pair<WindowHint, WindowHintValue>>& hints)
    : lh::Window::Window(resolution.first, resolution.second, name, fullscreen, hints)
{
}

lh::Window::Window() : lh::Window::Window(lh::Window::defaultFullscreen)
{
}

lh::Window::~Window()
{
    glfwDestroyWindow(glfwWindow);

    glfwTerminate();
}

lh::Window::WindowDimension lh::Window::GetWidth() const
{
    return width;
}

void lh::Window::SetWidth(WindowDimension value)
{
    width = value;
    glfwSetWindowSize(glfwWindow, width, GetHeight());
}

lh::Window::WindowDimension lh::Window::GetHeight() const
{
    return height;
}

void lh::Window::SetHeight(WindowDimension value)
{
    height = value;
    glfwSetWindowSize(glfwWindow, width, GetHeight());
}

lh::Window::WindowPosition lh::Window::GetWindowPosition() const
{
    std::pair<int, int> position {};

    glfwGetWindowPos(glfwWindow, &position.first, &position.second);

    return position;
}

void lh::Window::SetWindowPosition(WindowPosition position) const
{
    glfwSetWindowPos(glfwWindow, position.first, position.second);
}

std::string_view lh::Window::GetTitle() const
{
    return name;
}

void lh::Window::SetTitle(std::string_view title)
{
    name = title;
    glfwSetWindowTitle(glfwWindow, title.data());
}

void lh::Window::SetWindowSize(WindowResolution resolution)
{
    width = resolution.first;
    height = resolution.second;

    glfwSetWindowSize(glfwWindow, width, height);
}

GLFWwindow& lh::Window::GetGlfwWindow()
{
    return *glfwWindow;
}

double lh::Window::AspectRatio() const
{
    return width / height;
}

bool lh::Window::GetShouldClose() const
{
    return glfwWindowShouldClose(glfwWindow);
}

void lh::Window::SetShouldClose(bool value) const
{
    glfwSetWindowShouldClose(glfwWindow, value);
}

bool lh::Window::GetFullscreen() const
{
    return fullscreen;
}

void lh::Window::SetFullscreen(bool value)
{
    if (fullscreen == value)
        return;

    auto currentVideoMode = glfwGetVideoMode(glfwMonitor);

    // switch to windowed mode
    if (!value)
        glfwSetWindowMonitor(glfwWindow, nullptr, 0, 0, lh::Window::defaultWindowed.first,
                             lh::Window::defaultWindowed.second, currentVideoMode->refreshRate);

    // switch to fullscreen mode
    else
        glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, width, height, currentVideoMode->refreshRate);
}

void lh::Window::SetGamma(WindowGamma gamma) const
{
    glfwSetGamma(glfwMonitor, gamma);
}

void lh::Window::InitializeGLFW() const
{
    if (lh::Window::vkfwInitialzied)
        return;

    auto success = glfwInit();

    if (!success)
    {
        lh::Output::error() << "Could not initialize GLFW.";
        std::abort();
    }

    lh::Window::vkfwInitialzied = true;
}
