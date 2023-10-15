module;

#include "glm/glm.hpp"

module light;

namespace lh
{
	light::light(const colors::color& color, const intensity_t& intensity)
		: m_color {color.r, color.g, color.b, intensity}
	{}

	auto light::color() const -> const colors::color&
	{
		return m_color;
	}

	auto light::color(const colors::color& color) -> void
	{
		m_color = {color.r, color.g, color.b, m_color.a};
	}

	auto light::intensity() const -> const intensity_t&
	{
		return m_color.a;
	}

	auto light::intensity(const intensity_t& intensity) -> void
	{
		m_color.a = intensity;
	}

	// ===========================================================================

	physical_light::physical_light(const colors::color& color,
								   const light::intensity_t& intensity,
								   const entity::position_t& position,
								   const entity::rotation_t rotation,
								   const entity::scale_t scale)
		: light {color, intensity}, entity {position, rotation, scale}
	{}

	auto physical_light::intensity(const intensity_t& intensity) -> void
	{
		light::intensity(intensity);

		// given the inverse square intensity decay law, calculate the distance past which
		// this lights contribution can be discarded
		// threshold is an arbitrary value that is considered to no longer provide a significant contribution to final
		// lighting output
		m_effective_radius = glm::sqrt(m_color.a / physical_light_distance_threshold);
	}

	auto physical_light::effective_radius() const -> const light::intensity_t&
	{
		return m_effective_radius;
	}

	auto physical_light::intensity_at(const entity::position_t& position) const -> light::intensity_t
	{
		// physical light decay is proportional to inverse of the square of the distance between the source and point X
		return m_color.a * (1 / glm::pow(glm::distance(this->m_position, position), 2));
	}

	// ===========================================================================

	point_light::point_light(const lh::colors::color& color,
							 const light::intensity_t& intensity,
							 const entity::position_t& position)
		: physical_light {color, intensity, position}
	{}

	auto point_light::register_light() -> void
	{
		s_global_light_descriptor_buffer->m_point_light_data.push_back(
			{glm::vec4 {m_position, 1.0f}, glm::vec4 {m_color}});
	}

	global_light_descriptor_buffer::global_light_descriptor_buffer(const vulkan::physical_device& physical_device,
																   const vulkan::logical_device& logical_device,
																   const vulkan::memory_allocator& memory_allocator,
																   const create_info& create_info)
		: m_create_info {create_info}, m_light_info_data {}, m_point_light_data {}, m_light_resource_buffer {}
	{
		light::s_global_light_descriptor_buffer = this;

		const auto buffer_size = create_info.m_point_lights * sizeof point_light::shader_data +
								 create_info.m_spot_lights * sizeof spot_light::shader_data +
								 create_info.m_directional_lights * sizeof directional_light::shader_data +
								 create_info.m_ambient_lights * sizeof ambient_light::shader_data;

		auto buffer_subdata = decltype(vulkan::descriptor_resource_buffer::create_info::m_subdata) {};

		buffer_subdata.reserve(create_info.m_point_lights + create_info.m_spot_lights +
							   create_info.m_directional_lights + create_info.m_ambient_lights);

		const auto combined_lights_size = create_info.m_point_lights + create_info.m_spot_lights +
										  create_info.m_directional_lights + create_info.m_ambient_lights;

		auto buffer_offset = vk::DeviceSize {};

		for (auto i = create_info::light_size_t {}; i < combined_lights_size; i++)
		{
			const auto light_data_size = i >= create_info.m_point_lights + create_info.m_spot_lights +
													 create_info.m_directional_lights
											 ? sizeof ambient_light::shader_data
										 : i >= create_info.m_point_lights + create_info.m_spot_lights
											 ? sizeof directional_light::shader_data
										 : i >= create_info.m_point_lights ? sizeof spot_light::shader_data
																		   : sizeof point_light::shader_data;

			buffer_subdata.emplace_back(vk::DescriptorType::eStorageBuffer,
										vulkan::buffer_subdata::subdata {buffer_offset, light_data_size});

			buffer_offset += light_data_size;
		}

		m_light_resource_buffer = {physical_device,
								   logical_device,
								   memory_allocator,
								   buffer_size,
								   {{.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
												vk::BufferUsageFlagBits::eStorageBuffer},
									buffer_subdata}};
	}
}
