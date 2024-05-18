module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#include <map>
#include <filesystem>
#include <string>
#endif

export module user_interface;

import input;
import dear_imgui;
import vk_mem_alloc_hpp;

#if not INTELLISENSE
import std.core;
import std.filesystem;
#endif

export namespace lh
{
	class user_interface
	{
	public:
		struct create_info
		{
			dear_imgui::create_info m_dear_imgui_create_info = {};
		};

		user_interface(const window&,
					   const vulkan::instance&,
					   const vulkan::physical_device&,
					   const vulkan::logical_device&,
					   const vulkan::queue_families::family&,
					   const vulkan::queue&,
					   const vulkan::swapchain&,
					   const create_info& = {});

		user_interface(const user_interface&) = delete;
		user_interface operator=(const user_interface&) = delete;

		auto dear_imgui() -> lh::dear_imgui&;
		auto dear_imgui() const -> const lh::dear_imgui&;
		auto operator*() -> lh::dear_imgui&;
		auto operator*() const -> const lh::dear_imgui&;
		auto operator->() -> lh::dear_imgui&;
		auto operator->() const -> const lh::dear_imgui&;

		auto new_frame() const -> void;
		auto render(const vk::raii::CommandBuffer&) -> void;

		auto draw_crosshair() -> void;
		auto draw_gpu_statistics(const vma::TotalStatistics&) -> void;
		auto draw_gpu_budgets(const std::vector<vma::Budget>&) -> void;
		auto register_key_event(const input::key_binding::key_input&, const action&) -> void;

	private:
		const window& m_window;

		lh::dear_imgui m_dear_imgui;

		std::vector<action> m_actions;
	};
}