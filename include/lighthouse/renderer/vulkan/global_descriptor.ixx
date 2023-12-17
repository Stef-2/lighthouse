module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <cstdint>
#endif

export module global_descriptor;

import physical_device;
import logical_device;

#if not INTELLISENSE
import std.core;
import vulkan;
#endif

export namespace lh
{
	namespace vulkan
	{
		class global_descriptor
		{
		public:
			using descriptor_type_size_t = std::uint32_t;

			struct create_info
			{
				descriptor_type_size_t m_num_uniform_buffers = 15;
				descriptor_type_size_t m_num_storage_descriptors = 8;
				descriptor_type_size_t m_num_combined_image_samplers = 1024;
			};

			global_descriptor(const physical_device&, const logical_device&, const create_info& = {});
							  

			auto uniform_buffer_set() const -> const vk::raii::DescriptorSetLayout&;
			auto storage_descriptor_set() const -> const vk::raii::DescriptorSetLayout&;
			auto combined_image_sampler_set() const -> const vk::raii::DescriptorSetLayout&;

			auto descriptor_set_layouts() const -> const std::vector<vk::DescriptorSetLayout>;

			auto pipeline_layout() const -> const vk::raii::PipelineLayout&;

		private:
			descriptor_type_size_t m_num_uniform_buffers;
			vk::raii::DescriptorSetLayout m_uniform_buffer_set;

			descriptor_type_size_t m_num_storage_descriptors;
			vk::raii::DescriptorSetLayout m_storage_descriptor_set;

			descriptor_type_size_t m_num_combined_image_samplers;
			vk::raii::DescriptorSetLayout m_combined_image_sampler_set;

			vk::raii::PipelineLayout m_pipeline_layout;
		};
	}
}