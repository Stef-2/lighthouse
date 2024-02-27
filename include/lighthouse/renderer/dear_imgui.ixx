module;

#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#include <map>
#include <filesystem>
#include <string>
#endif

export module dear_imgui;

import lighthouse_string;
import window;
import instance;
import physical_device;
import logical_device;
import queue_families;
import queue;
import swapchain;
import texture;
import color;

#if not INTELLISENSE
import std.core;
import std.filesystem;
import vulkan_hpp;
#endif

export namespace lh
{
	class dear_imgui
	{
	public:
		struct font_info
		{
			std::filesystem::path m_font_path;
			ImFontConfig m_config = {};
			float m_size;
		};

		struct create_info
		{
			using texture_count_t = std::uint32_t;

			std::vector<font_info> m_font_infos = {};
			vk::SampleCountFlagBits m_rasterization_sample_count = vk::SampleCountFlagBits::e1;
			texture_count_t m_max_texture_count = 32;
		};

		dear_imgui(const window&,
				   const vulkan::instance&,
				   const vulkan::physical_device&,
				   const vulkan::logical_device&,
				   const vulkan::queue_families::family&,
				   const vulkan::queue&,
				   const vulkan::swapchain&,
				   const create_info& = {});

		dear_imgui(const dear_imgui&) = delete;
		dear_imgui& operator=(const dear_imgui&) = delete;
		dear_imgui(dear_imgui&&) noexcept = default;
		dear_imgui& operator=(dear_imgui&&) noexcept = default;

		~dear_imgui();

		auto new_frame() const -> void;
		auto render(const vk::raii::CommandBuffer&) -> void;

		auto push_font(const string::string_t&) const -> void;
		auto pop_font() const -> void;
		auto register_font(const font_info&) -> void;
		auto registered_fonts() const -> const std::vector<string::string_t>;

		auto register_texture(const vulkan::texture&) -> void;
		auto unregister_texture(const vulkan::texture&) -> void;
		auto registered_textures() const -> const std::vector<const vulkan::texture*>;

		auto push_color(ImGuiCol, const colors::color&) const -> void;
		auto pop_color() const -> void;
	private:
		create_info m_create_info;

		vk::raii::DescriptorPool m_descriptor_pool;
		ImGuiContext* m_context;
		std::map<string::string_t, ImFont*> m_font_map;
		std::map<const vulkan::texture*, vk::DescriptorSet> m_texture_map;
	};
}