module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <cstdint>
#endif

export module global_descriptor;

import physical_device;
import logical_device;
import memory_allocator;

#if not INTELLISENSE
import std.core;
import vulkan;
#endif

namespace lh
{
	namespace vulkan
	{
		class global_descriptor
		{
		public:
			struct create_info
			{
				using descriptor_type_size_t = std::uint16_t;

				descriptor_type_size_t m_num_uniform_buffers = 8;
				descriptor_type_size_t m_num_storage_descriptors = 8;
				descriptor_type_size_t m_num_combined_image_samplers = 8;

				vk::SamplerCreateInfo m_immutable_sampler_properties = {{},
																		vk::Filter::eNearest,
																		vk::Filter::eNearest,
																		vk::SamplerMipmapMode::eNearest,
																		vk::SamplerAddressMode::eRepeat,
																		vk::SamplerAddressMode::eRepeat,
																		vk::SamplerAddressMode::eRepeat,
																		0.0f,
																		false,
																		0.0f,
																		false,
																		vk::CompareOp::eNever,
																		0.0f,
																		0.0f,
																		vk::BorderColor::eFloatTransparentBlack,
																		false};
			};

			global_descriptor(const physical_device&,
							  const logical_device&,
							  const memory_allocator&,
							  const create_info& = {});

			auto uniform_buffer_set() const -> const vk::raii::DescriptorSetLayout&;
			auto storage_descriptor_set() const -> const vk::raii::DescriptorSetLayout&;
			auto combined_image_sampler_set() const -> const vk::raii::DescriptorSetLayout&;

		private:
			vk::raii::Sampler m_immutable_sampler;

			vk::raii::DescriptorSetLayout m_uniform_buffer_set;
			vk::raii::DescriptorSetLayout m_storage_descriptor_set;
			vk::raii::DescriptorSetLayout m_combined_image_sampler_set;

			vk::raii::PipelineLayout m_pipeline_layout;
		};
	}
}