module;

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

module imgui;

namespace lh
{
	imgui::imgui(const window& window,
				 const vulkan::instance& instance,
				 const vulkan::physical_device& physical_device,
				 const vulkan::logical_device& logical_device,
				 const vulkan::queue_families::family& queue_family,
				 const vulkan::queue& queue,
				 const vulkan::swapchain& swapchain,
				 const create_info& create_info)
		: m_descriptor_pool {nullptr}
	{
		const auto descriptor_pool_size = vk::DescriptorPoolSize {vk::DescriptorType::eCombinedImageSampler, 1};
		const auto descriptor_pool_create_info = vk::DescriptorPoolCreateInfo {
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1, descriptor_pool_size};
		m_descriptor_pool = {*logical_device, descriptor_pool_create_info};

		m_context = ImGui::CreateContext();

		auto vulkan_init_info =
			ImGui_ImplVulkan_InitInfo {**instance,
									   **physical_device,
									   **logical_device,
									   queue_family.m_index,
									   **queue,
									   {},
									   *m_descriptor_pool,
									   {},
									   2,
									   swapchain.image_count(),
									   static_cast<VkSampleCountFlagBits>(create_info.m_sample_count),
									   true,
									   static_cast<VkFormat>(swapchain.surface().format().surfaceFormat.format)};

		ImGui_ImplGlfw_InitForVulkan(window.vkfw_window(), true);
		ImGui_ImplVulkan_Init(&vulkan_init_info, {});
	}

	imgui::~imgui()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(m_context);
	}

	auto imgui::new_frame() const -> void
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	auto imgui::render(const vk::raii::CommandBuffer& command_buffer) -> void
	{
		bool wtf = true;
		ImGui::ShowDemoWindow(&wtf);
		ImGui::Render();

		const auto draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(draw_data, *command_buffer);
	}
}
