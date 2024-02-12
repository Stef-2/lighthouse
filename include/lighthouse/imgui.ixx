module;

#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module imgui;

import window;
import instance;
import physical_device;
import logical_device;
import queue_families;
import queue;
import swapchain;

#if not INTELLISENSE
import std.core;
import vulkan;
#endif

export namespace lh
{
	class imgui
	{
	public:
		struct create_info
		{
			vk::SampleCountFlagBits m_sample_count = vk::SampleCountFlagBits::e1;
		};

		imgui(const window&,
			  const vulkan::instance&,
			  const vulkan::physical_device&,
			  const vulkan::logical_device&,
			  const vulkan::queue_families::family&,
			  const vulkan::queue&,
			  const vulkan::swapchain&,
			  const create_info& = {});

		imgui(const imgui&) = delete;
		imgui& operator=(const imgui&) = delete;

		~imgui();

		auto new_frame() const -> void;
		auto render(const vk::raii::CommandBuffer&) -> void;

	private:
		vk::raii::DescriptorPool m_descriptor_pool;
		//vk::raii::Pipeline m_pipeline;
		//ImGui_ImplVulkanH_Window m_window;
		ImGuiContext* m_context;
	};
}