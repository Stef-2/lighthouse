#pragma once

#include "vkfw.hpp"

#include "Output.hpp"

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <map>

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

		// resolutions commonly used in video game engines
		enum class CommonResolutions
		{
			Res_320x200,
			Res_640x360,
			Res_800x600,
			Res_1024x768,
			Res_1280x720,
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
			Res_3840x2160,

			nHD					= Res_640x360,
			SVGA				= Res_800x600,
			XGA					= Res_1024x768,
			WXGA				= Res_1280x720,
			WXGA2				= Res_1280x800,
			SXGA				= Res_1280x1024,
			HD					= Res_1360x768,
			HD2					= Res_1366x768,
			WXGA3				= Res_1440x900,
			HD3					= Res_1600x900,
			WSXGA2				= Res_1680x1050,
			FHD					= Res_1920x1080,
			WUXGA				= Res_1920x1200,
			QWXGA				= Res_2048x1152,
			QXGA				= Res_2048x1536,
			UWFHD				= Res_2560x1080,
			QHD					= Res_2560x1440,
			WQXGA				= Res_2560x1600,
			UWQHD				= Res_3440x1440,
			UHD					= Res_3840x2160,

			Default_windowed	= Res_1280x720,
			Default_fullscreen	= Res_1920x1200
		};

		// mapping between 
		static inline const std::map<CommonResolutions, WindowResolution> common_resolutions =
		{
			{CommonResolutions::Res_320x200,	{320, 200}},
			{CommonResolutions::Res_640x360,	{640, 360}},
			{CommonResolutions::Res_800x600,	{800, 600}},
			{CommonResolutions::Res_1024x768,	{1024, 768}},
			{CommonResolutions::Res_1280x720,	{1280, 720}},
			{CommonResolutions::Res_1280x800,	{1280, 800}},
			{CommonResolutions::Res_1280x1024,	{1280, 1024}},
			{CommonResolutions::Res_1360x768,	{1360, 768}},
			{CommonResolutions::Res_1366x768,	{1366, 768}},
			{CommonResolutions::Res_1440x900,	{1440, 900}},
			{CommonResolutions::Res_1536x864,	{1536, 864}},
			{CommonResolutions::Res_1600x900,	{1600, 900}},
			{CommonResolutions::Res_1680x1050,	{1680, 1050}},
			{CommonResolutions::Res_1920x1080,	{1920, 1080}},
			{CommonResolutions::Res_1920x1200,	{1920, 1200}},
			{CommonResolutions::Res_2048x1152,	{2048, 1152}},
			{CommonResolutions::Res_2048x1536,	{2048, 1536}},
			{CommonResolutions::Res_2560x1080,	{2560, 1080}},
			{CommonResolutions::Res_2560x1440,	{2560, 1440}},
			{CommonResolutions::Res_2560x1600,	{2560, 1600}},
			{CommonResolutions::Res_3440x1440,	{3440, 1440}},
			{CommonResolutions::Res_3840x2160,	{3840, 2160}}
		};
		

		Window(WindowResolution resolution, std::string_view name = defaultName, bool fullscreen = false,
			   vkfw::WindowHints& = const_cast<vkfw::WindowHints&>(default_hints));
		Window();

		~Window();

		auto get_aspect_ratio() const -> double;
		auto get_title() const -> std::string_view;
		auto set_title(std::string_view) -> void;

		auto get_fullscreen() const -> bool;
		auto set_fullscreen(bool) -> void;

		auto vkfw_window() -> vkfw::Window&;
		auto vkfw_monitor() -> vkfw::Monitor&;

	private:

		auto initialize_vkfw() const -> void;

		static inline const auto default_hints = vkfw::WindowHints {
			.resizable = false,
			.decorated = true,
			.focused = true,
			.autoIconify = false,
			.centerCursor = true,
			.focusOnShow = true,
			.srgbCapable = true,
			.clientAPI = vkfw::ClientAPI::None
		};
		static inline auto defaultName = "Light House";
		static inline auto vkfwInitialzied = false;

		std::string title;
		vkfw::UniqueWindow window;
		vkfw::Monitor monitor;
	};
}
