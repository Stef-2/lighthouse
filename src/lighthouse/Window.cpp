#include "Window.hpp"

bool lh::Window::glfwInitialzied = false;

lh::Window::Window(WindowDimension width, WindowDimension height, std::string_view name, std::vector<std::pair<WindowHint, WindowHintValue>>& hints)
	:width(width), height(height), name(name)
{
	InitializeGLFW();

	// parse and apply window creation hints
	for (auto& [hint, value] : hints)
		glfwWindowHint(std::to_underlying(hint), std::to_underlying(value));

	// hint that disables OpenGL context creation and enables vulkan instead
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindow = glfwCreateWindow(width, height, this->name.c_str(), nullptr, nullptr);

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

GLFWwindow& lh::Window::GetGlfwWindow()
{
	return *glfwWindow;
}

bool lh::Window::ShouldClose()
{
	return glfwWindowShouldClose(glfwWindow);
}

void lh::Window::InitializeGLFW()
{
	if (lh::Window::glfwInitialzied)
		return;

	auto success = glfwInit();

	if (!success)
	{
		std::abort();
	}
	
	lh::Window::glfwInitialzied = true;
}