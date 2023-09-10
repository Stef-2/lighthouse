module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <cstdint>
#include <vector>
#endif

export module shader_input;

import lighthouse_string;

#if not INTELLISENSE
import vulkan;
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		struct shader_input
		{
			static inline constexpr auto s_remove_inactive_inputs = true;
			static inline constexpr auto s_stage_input_flag = vk::DescriptorType::eInputAttachment;

			enum class data_type
			{
				boolean,
				integer_16,
				unsigned_integer_16,
				integer_32,
				unsigned_integer_32,
				integer_64,
				unsigned_integer_64,
				float_16,
				float_32,
				float_64,
				structure,
				image,
				sampled_image,
				sampler
			};

			struct struct_member
			{
				data_type m_data_type;
				std::uint8_t m_rows;
				std::uint8_t m_colums;
				std::uint32_t m_array_dimension;
				vk::DeviceSize m_size;
				vk::DeviceSize m_offset;
			};

			auto operator()(const shader_input&) const -> std::size_t;
			auto operator==(const shader_input&) const -> bool;

			uint32_t m_descriptor_set;
			uint32_t m_descriptor_location;
			uint32_t m_descriptor_binding;

			vk::DescriptorType m_type;
			data_type m_data_type;
			std::uint8_t m_rows;
			std::uint8_t m_columns;
			std::uint32_t m_array_dimension;
			vk::DeviceSize m_size;

			std::vector<struct_member> m_members;
		};

		struct unique_pipeline_inputs
		{
			std::vector<shader_input> m_uniform_buffer_descriptors;
			std::vector<shader_input> m_combined_image_sampler_descriptors;
		};
	}
}}))...........