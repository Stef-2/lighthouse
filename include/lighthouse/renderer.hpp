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
		auto info(const create_info& = {}) -> string::string_t;

		vulkan::instance m_instance;

		vulkan::physical_device m_physical_device;
		vulkan::surface m_surface;

		vulkan::queue_families m_queue_families;

		vulkan::logical_device m_logical_device;
		vulkan::memory_allocator m_memory_allocator;
		vulkan::command_control m_command_control;

		vulkan::queue m_queue;
		vulkan::swapchain m_swapchain;

		vulkan::mapped_buffer m_uniform_buffer;

		vulkan::descriptor_set_layout m_descriptor_set_layout;
		vulkan::descriptor_collection m_descriptor_collection;

		vulkan::shader_object m_vertex_object;
		vulkan::shader_object m_fragment_object;

		vk::raii::PipelineLayout m_pipeline_layout;

		vulkan::mapped_buffer m_vertex_buffer;
	};
}
