module;

#include "imgui/imgui.h"
#include "vulkan/vma/vk_mem_alloc.h"

module user_interface;

import lighthouse_string;

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
		const auto x = static_cast<float>(m_window.resolution().width / 2);
		const auto y = static_cast<float>(m_window.resolution().height / 2);

		ImGui::GetForegroundDrawList()->AddLine(ImVec2 {x - 10, y}, ImVec2 {x + 10, y}, ImColor {255, 255, 255});
		ImGui::GetForegroundDrawList()->AddLine(ImVec2 {x, y - 10}, ImVec2 {x, y + 10}, ImColor {255, 255, 255});
	}

	auto user_interface::draw_gpu_statistics(const vma::TotalStatistics& statistics) -> void
	{
		ImGui::Begin("gpu statistics");

		// memory types
		for (auto i = std::uint32_t {0}; i < VK_MAX_MEMORY_TYPES; i++)
		{
			const auto header_text = lh::string::string_t {"memory type: " + std::to_string(i)};

			if (ImGui::CollapsingHeader(header_text.c_str(), ImGuiTreeNodeFlags_None))
			{
				ImGui::Text("block count: %d", statistics.memoryType[i].statistics.blockCount);
				ImGui::Text("allocation count: %d", statistics.memoryType[i].statistics.allocationCount);
				ImGui::Text("block bytes: %d", statistics.memoryType[i].statistics.blockBytes);
				ImGui::Text("allocation bytes: %d", statistics.memoryType[i].statistics.allocationBytes);

				ImGui::Text("unused range count: %d", statistics.memoryType[i].unusedRangeCount);
				ImGui::Text("allocation size min: %d", statistics.memoryType[i].allocationSizeMin);
				ImGui::Text("allocation size max: %d", statistics.memoryType[i].allocationSizeMax);
				ImGui::Text("unused range min: %d", statistics.memoryType[i].unusedRangeSizeMin);
				ImGui::Text("unused range max: %d", statistics.memoryType[i].unusedRangeSizeMax);
			}
		}

		// memory heaps
		for (auto i = std::uint32_t {0}; i < VK_MAX_MEMORY_HEAPS; i++)
		{
			const auto header_text = lh::string::string_t {"memory heap: " + std::to_string(i)};

			if (ImGui::CollapsingHeader(header_text.c_str(), ImGuiTreeNodeFlags_None))
			{
				ImGui::Text("block count: %d", statistics.memoryHeap[i].statistics.blockCount);
				ImGui::Text("allocation count: %d", statistics.memoryHeap[i].statistics.allocationCount);
				ImGui::Text("block bytes: %d", statistics.memoryHeap[i].statistics.blockBytes);
				ImGui::Text("allocation bytes: %d", statistics.memoryHeap[i].statistics.allocationBytes);

				ImGui::Text("unused range count: %d", statistics.memoryHeap[i].unusedRangeCount);
				ImGui::Text("allocation size min: %d", statistics.memoryHeap[i].allocationSizeMin);
				ImGui::Text("allocation size max: %d", statistics.memoryHeap[i].allocationSizeMax);
				ImGui::Text("unused range min: %d", statistics.memoryHeap[i].unusedRangeSizeMin);
				ImGui::Text("unused range max: %d", statistics.memoryHeap[i].unusedRangeSizeMax);
			}
		}

		// total
		if (ImGui::CollapsingHeader("total: ", ImGuiTreeNodeFlags_None))
		{
			ImGui::Text("block count: %d", statistics.total.statistics.blockCount);
			ImGui::Text("allocation count: %d", statistics.total.statistics.allocationCount);
			ImGui::Text("block bytes: %d", statistics.total.statistics.blockBytes);
			ImGui::Text("allocation bytes: %d", statistics.total.statistics.allocationBytes);

			ImGui::Text("unused range count: %d", statistics.total.unusedRangeCount);
			ImGui::Text("allocation size min: %d", statistics.total.allocationSizeMin);
			ImGui::Text("allocation size max: %d", statistics.total.allocationSizeMax);
			ImGui::Text("unused range min: %d", statistics.total.unusedRangeSizeMin);
			ImGui::Text("unused range max: %d", statistics.total.unusedRangeSizeMax);
		}

		ImGui::End();
	}

	auto user_interface::register_key_event(const input::key_binding::key_input& key_input, const action& action)
		-> void
	{}
}
