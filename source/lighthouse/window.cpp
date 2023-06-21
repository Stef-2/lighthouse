#include "lighthouse/window.hpp"
#include "lighthouse/output.hpp"

lh::window::window(const create_info& create_info) : m_title(create_info.m_name)
{
	initialize_vkfw();

	// set the monitor depending on fullscreen mode
	m_monitor = create_info.m_fullscreen ? vkfw::getPrimaryMonitor().value : nullptr;

	auto result = vkfw::Result {};
	std::tie(result, m_window) = vkfw::createWindowUnique(create_info.m_resolution.width,
														  create_info.m_resolution.height,
														  create_info.m_name,
														  create_info.m_hints,
														  m_monitor)
									 .asTuple();

	if (not vkfw::check(result))
		output::fatal() << "Could not initialize VKFW / GLFW window";
}

lh::window::window(const window&& other) noexcept : window(const_cast<window&&>(other)) {}

auto lh::window::resolution() const -> window_resolution_t
{
	auto [width, height] {m_window.get().getFramebufferSize().value};

	return window_resolution_t {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

auto lh::window::aspect_ratio() const -> double
{
	auto&& [width, height] = m_window.get().getFramebufferSize().value;

	return static_cast<double>(width) / static_cast<double>(height);
}

auto lh::window::title() const -> std::string_view
{
	return m_title;
}

auto lh::window::title(std::string_view value) -> void
{
	m_title = value;
	m_window.get().setTitle(value);
}

auto lh::window::fullscreen() const -> bool
{
	return m_monitor;
}

auto lh::window::fullscreen(bool value) -> void
{
	if (fullscreen() == value)
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

	auto initialize = []() {
		auto success = vkfw::init();

		if (!vkfw::check(success))
		{
			lh::output::fatal() << "Could not initialize VKFW / GLFW.";
		}
	};

	std::call_once(once, initialize);
}
