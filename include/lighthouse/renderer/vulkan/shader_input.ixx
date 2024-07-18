module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif

export module shader_input;

import lighthouse_string;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import std;

export namespace lh
{
	namespace vulkan
	{
		struct shader_input
		{
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
				sampler,
				invalid
			};

			enum class storage_class
			{
				uniform_constant,
				input,
				uniform,
				output,
				workgroup,
				cross_workgroup,
				private_storage,
				function,
				generic,
				push_constant,
				atomic_counter,
				image,
				storage_buffer,
				callable_data_KHR,
				incoming_callable_data_KHR,
				ray_payload_KHR,
				hit_attribute_KHR,
				incoming_ray_payload_KHR,
				shader_record_buffer_KHR,
				physical_storage_buffer,
				task_payload_workgroup_EXT,
				invalid
			};

			struct struct_member
			{
				data_type m_data_type;
				std::uint8_t m_rows;
				std::uint8_t m_colums;
				std::uint32_t m_array_dimension;
				vk::DeviceSize m_size;
				std::uint32_t m_offset;
			};

			auto operator()(const shader_input&) const -> std::size_t;
			auto operator==(const shader_input&) const -> bool;

			std::uint32_t m_descriptor_set;
			std::uint32_t m_descriptor_location;
			std::uint32_t m_descriptor_binding;

			vk::DescriptorType m_type;
			storage_class m_storage_class;
			data_type m_data_type;
			std::uint8_t m_rows;
			std::uint8_t m_columns;
			std::uint32_t m_array_dimension;
			vk::DeviceSize m_size;
			std::uint32_t m_set_offset;
			std::uint32_t m_binding_offset;
			std::uint32_t m_location_offset;

			std::vector<struct_member> m_members;
		};

		struct unique_pipeline_inputs
		{
			std::vector<shader_input> m_uniform_buffer_descriptors;
			std::vector<shader_input> m_storage_buffer_descriptors;
			std::vector<shader_input> m_combined_image_sampler_descriptors;
			//shader_input m_push_constant;
		};
	}
}