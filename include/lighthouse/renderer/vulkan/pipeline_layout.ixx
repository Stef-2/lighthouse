module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module pipeline_layout;

import physical_device;
import logical_device;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import std;

export namespace lh
{
	namespace vulkan
	{
		class pipeline_layout
		{
		public:
			using descriptor_type_size_t = std::uint32_t;

			struct create_info
			{
				descriptor_type_size_t m_num_uniform_buffers = 32;
				descriptor_type_size_t m_num_storage_buffers = 8;
				descriptor_type_size_t m_num_combined_image_samplers = 1024;
			};

			pipeline_layout(const physical_device&, const logical_device&, const create_info& = {});


			auto uniform_buffer_set() const -> const vk::raii::DescriptorSetLayout&;
			auto storage_buffer_set() const -> const vk::raii::DescriptorSetLayout&;
			auto combined_image_sampler_set() const -> const vk::raii::DescriptorSetLayout&;

			auto descriptor_set_layouts() const -> const std::array<vk::DescriptorSetLayout, 3>;

			auto push_constant_range() const -> const vk::PushConstantRange&;
			auto layout() const -> const vk::raii::PipelineLayout&;

			auto create_information() const -> const create_info&;

		private:
			create_info m_create_info;

			vk::raii::DescriptorSetLayout m_uniform_buffer_set;
			vk::raii::DescriptorSetLayout m_storage_buffer_set;
			vk::raii::DescriptorSetLayout m_combined_image_sampler_set;

			vk::PushConstantRange m_push_constant_range;
			vk::raii::PipelineLayout m_pipeline_layout;
		};
	}
}