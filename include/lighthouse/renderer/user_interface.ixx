module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#include <map>
#include <filesystem>
#include <string>
#endif

export module user_interface;

import dear_imgui;

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

		auto dear_imgui() -> lh::dear_imgui&;

	private:
		lh::dear_imgui m_dear_imgui;
	};
}