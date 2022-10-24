#pragma once

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

#include "vulkan.hpp"

#include <utility>
#include <vector>
#include <string>
#include <memory>

namespace lh
{
	// class that encapsulates GLFW window and monitor, vulkan device, instance and surface
	class Window
	{
	public:

		using WindowDimension = unsigned int;

		enum class WindowHint
		{
			Resizeable				= GLFW_RESIZABLE,
			Visible					= GLFW_VISIBLE,
			Decorated				= GLFW_DECORATED,
			Focused					= GLFW_FOCUSED,
			AutoIconify				= GLFW_AUTO_ICONIFY,
			Floating				= GLFW_FLOATING,
			Maximized				= GLFW_MAXIMIZED,
			CenterCursor			= GLFW_CENTER_CURSOR,
			TransparentFrameBuffer	= GLFW_TRANSPARENT_FRAMEBUFFER,
			FocusOnShow				= GLFW_FOCUS_ON_SHOW,
			ScaleToMonitor			= GLFW_SCALE_TO_MONITOR
		};

		enum class WindowHintValue
		{
			DontCare				= GLFW_DONT_CARE,
			True					= GLFW_TRUE,
			False					= GLFW_FALSE
		};

		Window(WindowDimension width, WindowDimension height, std::string_view name, std::vector<std::pair<WindowHint, WindowHintValue>>& hints = defaultHints);

		~Window();

		lh::Window::WindowDimension GetWidth() const;
		void SetWidth(lh::Window::WindowDimension value);

		lh::Window::WindowDimension GetHeight() const;
		void SetHeight(lh::Window::WindowDimension value);

		GLFWwindow& GetGlfwWindow();

		bool ShouldClose();

	private:

		void InitializeGLFW();

		static inline std::vector<std::pair<WindowHint, WindowHintValue>> defaultHints = 
		{
			{ WindowHint::Resizeable, WindowHintValue::False },
			{ WindowHint::Focused, WindowHintValue::True },
			{ WindowHint::CenterCursor, WindowHintValue::True },
			{ WindowHint::ScaleToMonitor, WindowHintValue::True }
		};

		WindowDimension width;
		WindowDimension height;
		std::string name;

		GLFWwindow* glfwWindow;
		GLFWmonitor* glfwMonitor;

		static bool glfwInitialzied;
	};
}