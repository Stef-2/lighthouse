#pragma once

#include "lighthouse/vulkan/vulkan.hpp"

#include "lighthouse/version.hpp"

#include <iterator>
#include <ranges>
#include <vector>

namespace lh
{
	class renderer
	{
	public:
		struct create_info
		{
			version m_engine_version = version::m_engine_version;
			version m_vulkan_version = version::m_vulkan_version;
			uint32_t m_frames_in_flight = 2;
			bool m_using_validation = true;
		};

		renderer(const window&, const create_info& = {});

		auto render() -> void;

	private:
		auto create_depth_buffer(const window&) -> vk::raii::ImageView;
		auto create_depth_buffer_data(const window&) -> vk::raii::su::DepthBufferData;
		auto create_uniform_buffer() -> vk::raii::su::BufferData;
		auto create_descriptor_set_layout() -> vk::raii::DescriptorSetLayout;
		auto create_pipeline_layout() -> vk::raii::PipelineLayout;
		auto create_format() -> vk::Format;

		auto create_vertex_buffer() -> vk::raii::su::BufferData;

		auto create_pipeline_cache() -> vk::raii::PipelineCache;

		auto info(const create_info& = {}) -> string::string_t;

		template <typename T>
			requires std::is_same_v<T, vulkan::vk_layers_t> || std::is_same_v<T, vulkan::vk_extensions_t>
		static auto assert_required_components(T& supported, const vulkan::vk_string_t& required_components) -> bool
		{

			for (const auto& required : required_components)
			{
				if (std::find_if(supported.begin(), supported.end(), [&required](const auto& supported) {
						auto name = std::string {};
						if constexpr (std::is_same_v<T, vulkan::vk_layers_t>)
							name = supported.layerName.data();
						else
							name = supported.extensionName.data();

						return strcmp(required, name.c_str()) == 0;
					}) == supported.end())
				{
					output::error() << "this system does not support the required vulkan component: " +
										   std::string {required};
					return false;
				}
			}

			return true;
		};

		vulkan::instance m_instance;

		vulkan::physical_device m_physical_device;
		vulkan::surface m_surface;

		vulkan::queue_families m_queue_families;

		vulkan::logical_device m_device;
		vulkan::memory_allocator m_memory_allocator;
		vulkan::command_control m_command_control;

		vulkan::queue m_queue;
		vulkan::image m_dyn_rend_image;
		vulkan::swapchain m_swapchain;

		vulkan::mapped_buffer m_uniform_buffer;

		vulkan::descriptor_set_layout m_descriptor_set_layout;
		vulkan::descriptor_set_layout m_temp_buffered_dsl;
		vulkan::descriptor_collection m_descriptor_collection;

		vulkan::shader_object m_vertex_object;
		vulkan::shader_object m_fragment_object;

		vk::raii::PipelineLayout m_pipeline_layout;

		vk::raii::su::BufferData m_vertex_buffer;
	};
}
