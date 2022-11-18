#pragma once

#include "vkfw.hpp"

#include "Output.hpp"

#include <utility>
#include <vector>
#include <string>
#include <memory>

namespace lh
{
	// class that encapsulates GLFW / VKFW window and monitor for display
	class Window
	{
	public:

		using WindowDimension = unsigned int;
		using WindowGamma = float;
		using WindowResolution = std::pair<WindowDimension, WindowDimension>;
		using WindowPosition = std::pair<WindowDimension, WindowDimension>;

		// relevant subset of glfw window hints
		enum class WindowHint
		{
			Resizeable				= std::to_underlying(vkfw::WindowHint::Resizable),
			Visible					= std::to_underlying(vkfw::WindowHint::Visible),
			Decorated				= std::to_underlying(vkfw::WindowHint::Decorated),
			Focused					= std::to_underlying(vkfw::WindowHint::Focused),
			AutoIconify				= std::to_underlying(vkfw::WindowHint::AutoIconify),
			Floating				= std::to_underlying(vkfw::WindowHint::Floating),
			Maximized				= std::to_underlying(vkfw::WindowHint::Maximized),
			CenterCursor			= std::to_underlying(vkfw::WindowHint::CenterCursor),
			FocusOnShow				= std::to_underlying(vkfw::WindowHint::FocusOnShow),
			ScaleToMonitor			= std::to_underlying(vkfw::WindowHint::ScaleToMonitor),

		};

		enum class WindowHintValue
		{
			DontCare				= std::to_underlying(vkfw::DontCare),
			True					= std::to_underlying(vkfw::True),
			False					= std::to_underlying(vkfw::False)
		};

		enum class CommonResolutions
		{
			Res_320x200,
			Res_640x360,
			Res_1024x768,
			Res_1270x720,
			Res_1280x800,
			Res_1280x1024,
			Res_1360x768,
			Res_1366x768,
			Res_1440x900,
			Res_1536x864,
			Res_1600x900,
			Res_1680x1050,
			Res_1920x1080,
			Res_1920x1200,
			Res_2048x1152,
			Res_2048x1536,
			Res_2560x1080,
			Res_2560x1440,
			Res_2560x1600,
			Res_3440x1440,
			Res_3840x2160
		};

		static inline const std::vector<WindowResolution> commonResolutions =
		{
			{320, 200},
			{640, 360},
			{1024, 768},
			{1270, 720},
			{1280, 800},
			{1280, 1024},
			{1360, 768},
			{1366, 768},
			{1440, 900},
			{1536, 864},
			{1600, 900},
			{1680, 1050},
			{1920, 1080},
			{1920, 1200},
			{2048, 1152},
			{2048, 1536},
			{2560, 1080},
			{2560, 1440},
			{2560, 1600},
			{3440, 1440},
			{3840, 2160}
		};
		

		Window(WindowDimension width, WindowDimension height, std::string_view name = defaultName, bool fullscreen = false, const std::vector<std::pair<WindowHint, WindowHintValue>>& hints = defaultHints);
		Window(WindowResolution resolution, std::string_view name = defaultName, bool fullscreen = false, const std::vector<std::pair<WindowHint, WindowHintValue>>& hints = defaultHints);
		Window();

		~Window();

		WindowDimension GetWidth() const;
		void SetWidth(WindowDimension value);

		WindowDimension GetHeight() const;
		void SetHeight(WindowDimension value);

		WindowPosition GetWindowPosition() const;
		void SetWindowPosition(WindowPosition) const;

		std::string_view GetTitle() const;
		void SetTitle(std::string_view);

		void SetWindowSize(WindowResolution);

		GLFWwindow& GetGlfwWindow();

		double AspectRatio() const;

		bool GetShouldClose() const;
		void SetShouldClose(bool) const;

		bool GetFullscreen() const;
		void SetFullscreen(bool);

		void SetGamma(WindowGamma) const;

	private:

		void InitializeGLFW() const;

		static inline const std::vector<std::pair<WindowHint, WindowHintValue>> defaultHints = 
		{
			{WindowHint::Resizeable, WindowHintValue::False},
			{WindowHint::Focused, WindowHintValue::True},
			{WindowHint::CenterCursor, WindowHintValue::True},
			{WindowHint::ScaleToMonitor, WindowHintValue::False}
		};

		static inline const char* defaultName = "Light House";
		static inline WindowResolution defaultFullscreen = {1920, 1080};
		static inline WindowResolution defaultWindowed = {800, 600};

		bool fullscreen;
		WindowDimension width;
		WindowDimension height;
		std::string name;

		vkfw::UniqueWindow window;
		GLFWwindow* glfwWindow;
		GLFWmonitor* glfwMonitor;
		GLFWvidmode glfwVideoMode;

		static bool vkfwInitialzied;
	};
}