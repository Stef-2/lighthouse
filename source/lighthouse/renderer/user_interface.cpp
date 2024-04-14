module;

#include "imgui/imgui.h"

module user_interface;

namespace lh
{
	user_interface::user_interface(const window& window,
								   const vulkan::instance& instance,
								   const vulkan::physical_device& physical_device,
								   const vulkan::logical_device& logical_device,
								   const vulkan::queue_families::family& queue_family,
								   const vulkan::queue& queue,
								   const vulkan::swapchain& swapchain,
								   const create_info& create_info)
		: m_window {window},
		  m_dear_imgui {window,
						instance,
						physical_device,
						logical_device,
						queue_family,
						queue,
						swapchain,
						create_info.m_dear_imgui_create_info}
	{}

	auto user_interface::dear_imgui() -> lh::dear_imgui&
	{
		return m_dear_imgui;
	}

	auto user_interface::dear_imgui() const -> const lh::dear_imgui&
	{
		return m_dear_imgui;
	}

	auto user_interface::operator*() -> lh::dear_imgui&
	{
		return m_dear_imgui;
	}

	auto user_interface::operator*() const -> const lh::dear_imgui&
	{
		return m_dear_imgui;
	}

	auto user_interface::operator->() -> lh::dear_imgui&
	{
		return m_dear_imgui;
	}

	auto user_interface::operator->() const -> const lh::dear_imgui&
	{
		return m_dear_imgui;
	}

	auto user_interface::new_frame() const -> void
	{
		m_dear_imgui.new_frame();
	}

	auto user_interface::render(const vk::raii::CommandBuffer& command_buffer) -> void
	{
		m_dear_imgui.render(command_buffer);
	}

	auto user_interface::draw_crosshair() -> void
	{
		const auto x = (float)m_window.resolution().width / 2;
		const auto y = (float)m_window.resolution().height / 2;

		ImGui::GetForegroundDrawList()->AddLine(ImVec2 {x - 10, y}, ImVec2 {x + 10, y}, ImColor {255, 255, 255});
		ImGui::GetForegroundDrawList()->AddLine(ImVec2 {x, y - 10}, ImVec2 {x, y + 10}, ImColor {255, 255, 255});
	}

	auto user_interface::register_key_event(const input::key_binding::key_input& key_input, const action& action)
		-> void
	{}
}
