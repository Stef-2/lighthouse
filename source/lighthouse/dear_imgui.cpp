module;

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

module dear_imgui;

namespace lh
{
	dear_imgui::dear_imgui(const window& window,
						   const vulkan::instance& instance,
						   const vulkan::physical_device& physical_device,
						   const vulkan::logical_device& logical_device,
						   const vulkan::queue_families::family& queue_family,
						   const vulkan::queue& queue,
						   const vulkan::swapchain& swapchain,
						   const create_info& create_info)
		: m_create_info {create_info}, m_descriptor_pool {nullptr}, m_font_map {}, m_texture_map {}
	{
		// create a descriptor pool to be used by imgui
		const auto descriptor_pool_size = vk::DescriptorPoolSize {vk::DescriptorType::eCombinedImageSampler,
																  create_info.m_max_texture_count};
		const auto descriptor_pool_create_info = vk::DescriptorPoolCreateInfo {
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1, descriptor_pool_size};
		m_descriptor_pool = {*logical_device, descriptor_pool_create_info};

		// create context
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
									   static_cast<VkSampleCountFlagBits>(create_info.m_rasterization_sample_count),
									   true,
									   static_cast<VkFormat>(swapchain.surface().format().surfaceFormat.format)};

		// initialize
		ImGui_ImplGlfw_InitForVulkan(window.vkfw_window(), true);
		ImGui_ImplVulkan_Init(&vulkan_init_info, {});

		// generate fonts if their paths were provided
		for (const auto& imgui_io = ImGui::GetIO(); const auto& font_info : create_info.m_font_infos)
		{
			const auto font_name = font_info.m_font_path.stem().string();
			m_font_map.emplace(font_name,
							   imgui_io.Fonts->AddFontFromFileTTF(font_info.m_font_path.string().c_str(),
																  font_info.m_size,
																  &font_info.m_config));
		}
	}

	dear_imgui::~dear_imgui()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(m_context);
	}

	auto dear_imgui::new_frame() const -> void
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	auto dear_imgui::render(const vk::raii::CommandBuffer& command_buffer) -> void
	{
		bool wtf = true;
		ImGui::ShowDemoWindow(&wtf);
		ImGui::Render();

		const auto draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(draw_data, *command_buffer);
	}

	auto dear_imgui::push_font(const string::string_t& font_name) const -> void
	{
		ImGui::PushFont(m_font_map.at(font_name));
	}

	auto dear_imgui::pop_font() const -> void
	{
		ImGui::PopFont();
	}

	auto dear_imgui::register_font(const font_info& font_info) -> void
	{
		const auto& imgui_io = ImGui::GetIO();

		m_font_map.emplace(font_info.m_font_path.stem().string(),
						   imgui_io.Fonts->AddFontFromFileTTF(font_info.m_font_path.string().c_str(),
															  font_info.m_size,
															  &font_info.m_config));
	}

	auto dear_imgui::registered_fonts() const -> const std::vector<string::string_t>
	{
		auto registered_fonts = decltype(dear_imgui::registered_fonts()) {};
		registered_fonts.reserve(m_font_map.size());

		for (const auto& [name, font] : m_font_map)
			registered_fonts.emplace_back(name);

		return registered_fonts;
	}

	auto dear_imgui::register_texture(const vulkan::texture& texture) -> void
	{
		m_texture_map.emplace(&texture,
							  ImGui_ImplVulkan_AddTexture(**texture.sampler(),
														  **texture.view(),
														  static_cast<VkImageLayout>(
															  vk::ImageLayout::eShaderReadOnlyOptimal)));
	}

	auto dear_imgui::unregister_texture(const vulkan::texture& texture) -> void
	{
		const auto texture_iterator = m_texture_map.find(&texture);

		if (texture_iterator != m_texture_map.end())
			m_texture_map.erase(texture_iterator);
	}
	auto dear_imgui::registered_textures() const -> const std::vector<const vulkan::texture*>
	{
		auto registered_textures = decltype(dear_imgui::registered_textures()) {};
		registered_textures.reserve(m_texture_map.size());

		for (const auto& [texture, descriptor_set] : m_texture_map)
			registered_textures.emplace_back(texture);

		return registered_textures;
	}
}
