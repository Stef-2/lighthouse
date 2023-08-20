module;
#include "vkfw/vkfw.hpp"

#if INTELLISENSE
#include <map>
#endif

export module window;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#else
import vulkan;
import std.core;
#endif
import lighthouse_string;

export namespace lh
{
	// class that encapsulates GLFW / VKFW window and monitor for display
	class window
	{
	public:
		using window_dimension_t = uint32_t;
		using window_gamma_t = float;
		using window_resolution_t = vk::Extent2D;
		using window_position_t = std::pair<window_dimension_t, window_dimension_t>;

		// resolutions commonly used in video game engines
		// clang-format off
		enum class common_resolutions
		{
			res_320x200,
			res_640x360,
			res_800x600,
			res_1024x768,
			res_1280x720,
			res_1280x800,
			res_1280x1024,
			res_1360x768,
			res_1366x768,
			res_1440x900,
			res_1536x864,
			res_1600x900,
			res_1680x1050,
			res_1920x1080,
			res_1920x1200,
			res_2048x1152,
			res_2048x1536,
			res_2560x1080,
			res_2560x1440,
			res_2560x1600,
			res_3440x1440,
			res_3840x2160,

			nHD					= res_640x360,
			SVGA				= res_800x600,
			XGA					= res_1024x768,
			WXGA				= res_1280x720,
			WXGA2				= res_1280x800,
			SXGA				= res_1280x1024,
			HD					= res_1360x768,
			HD2					= res_1366x768,
			WXGA3				= res_1440x900,
			HD3					= res_1600x900,
			WSXGA2				= res_1680x1050,
			FHD					= res_1920x1080,
			WUXGA				= res_1920x1200,
			QWXGA				= res_2048x1152,
			QXGA				= res_2048x1536,
			UWFHD				= res_2560x1080,
			QHD					= res_2560x1440,
			WQXGA				= res_2560x1600,
			UWQHD				= res_3440x1440,
			UHD					= res_3840x2160,

			default_windowed	= res_1280x720,
			default_fullscreen	= res_1920x1200
		};

		static inline const std::map<common_resolutions, window_resolution_t> m_resolution =
		{
			{common_resolutions::res_320x200,	{320, 200}},
			{common_resolutions::res_640x360,	{640, 360}},
			{common_resolutions::res_800x600,	{800, 600}},
			{common_resolutions::res_1024x768,	{1024, 768}},
			{common_resolutions::res_1280x720,	{1280, 720}},
			{common_resolutions::res_1280x800,	{1280, 800}},
			{common_resolutions::res_1280x1024,	{1280, 1024}},
			{common_resolutions::res_1360x768,	{1360, 768}},
			{common_resolutions::res_1366x768,	{1366, 768}},
			{common_resolutions::res_1440x900,	{1440, 900}},
			{common_resolutions::res_1536x864,	{1536, 864}},
			{common_resolutions::res_1600x900,	{1600, 900}},
			{common_resolutions::res_1680x1050,	{1680, 1050}},
			{common_resolutions::res_1920x1080,	{1920, 1080}},
			{common_resolutions::res_1920x1200,	{1920, 1200}},
			{common_resolutions::res_2048x1152,	{2048, 1152}},
			{common_resolutions::res_2048x1536,	{2048, 1536}},
			{common_resolutions::res_2560x1080,	{2560, 1080}},
			{common_resolutions::res_2560x1440,	{2560, 1440}},
			{common_resolutions::res_2560x1600,	{2560, 1600}},
			{common_resolutions::res_3440x1440,	{3440, 1440}},
			{common_resolutions::res_3840x2160,	{3840, 2160}}
		};
		//clang-format on

		struct create_info
		{
			window_resolution_t m_resolution = window::m_resolution.at(common_resolutions::default_windowed);
			const char* m_name = "lighthouse";
			bool m_fullscreen = false;
			vkfw::WindowHints m_hints = {
				.resizable = false,
				.decorated = true,
				.focused = true,
				.autoIconify = false,
				.centerCursor = true,
				.focusOnShow = true,
				.srgbCapable = true,
				.clientAPI = vkfw::ClientAPI::None
			};
		};

		window(const create_info& = {});

		// move constructible only
		window(const window&&) noexcept;
		window(window&&) noexcept = default;

		auto aspect_ratio() const -> double;
		auto resolution() const -> window_resolution_t;
		auto title() const -> std::string_view;
		auto title(std::string_view) -> void;

		auto fullscreen() const -> bool;
		auto fullscreen(bool) -> void;

		auto vkfw_window() const -> vkfw::Window&;
		auto vkfw_monitor() -> vkfw::Monitor&;

	private:
		auto initialize_vkfw() const -> void;

		mutable vkfw::UniqueWindow m_window;
		vkfw::Monitor m_monitor;
		string::string_t m_title;
	};
}
