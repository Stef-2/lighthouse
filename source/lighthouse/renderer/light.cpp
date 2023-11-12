module;

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#if INTELLISENSE
#include <ranges>
#endif

module light;

import vulkan_utility;

namespace lh
{
	light::light(const colors::color& color, const intensity_t& intensity)
		: m_color {color.r, color.g, color.b, intensity}
	{}

	auto light::color() const -> const colors::color&
	{
		return m_color;
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

	auto physical_light::color(const colors::color& color) -> void
	{
		m_color = {color.r, color.g, color.b, m_color.a};
		update_light_on_stack();
	}

	auto physical_light::intensity(const intensity_t& intensity) -> void
	{
		light::intensity(intensity);
		update_light_on_stack();

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

	auto physical_light::on_position_change() -> void
	{
		if (dynamic_cast<point_light*>(this))
		{
			update_light_on_stack();
			s_global_light_descriptor_buffer->map_point_lights();
		} else if (dynamic_cast<spot_light*>(this))
		{
			update_light_on_stack();
			s_global_light_descriptor_buffer->map_spot_lights();
		} else if (dynamic_cast<directional_light*>(this))
		{
			update_light_on_stack();
			s_global_light_descriptor_buffer->map_directional_lights();
		} else
		{
			update_light_on_stack();
			s_global_light_descriptor_buffer->map_ambient_lights();
		}
	}

	auto physical_light::on_rotation_change() -> void
	{
		if (dynamic_cast<spot_light*>(this))
		{
			update_light_on_stack();
			s_global_light_descriptor_buffer->map_spot_lights();
		} else if (dynamic_cast<directional_light*>(this))
		{
			update_light_on_stack();
			s_global_light_descriptor_buffer->map_directional_lights();
		}
	}

	// ===========================================================================

	point_light::point_light(const lh::colors::color& color,
							 const light::intensity_t& intensity,
							 const entity::position_t& position)
		: physical_light {color, intensity, position}
	{
		s_global_light_descriptor_buffer->m_point_lights.push_back(this);
		s_global_light_descriptor_buffer->m_point_light_data.emplace_back(glm::vec4 {m_position, 1.0f}, m_color);
		m_light_stack_index = s_global_light_descriptor_buffer->m_point_lights.size() - 1;
		s_global_light_descriptor_buffer->map_point_lights();
	}

	auto point_light::update_light_on_stack() -> void
	{
		s_global_light_descriptor_buffer->m_point_light_data[m_light_stack_index] = {glm::vec4 {m_position, 1.0f},
																					 m_color};
		s_global_light_descriptor_buffer->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f}, m_color}, m_light_stack_index * sizeof shader_data);
	}

	auto point_light::remove_light_from_stack() -> void
	{
		s_global_light_descriptor_buffer->m_point_light_data.erase(
			s_global_light_descriptor_buffer->m_point_light_data.begin() + m_light_stack_index);

		const auto subsequent_elements =
			std::ranges::subrange {s_global_light_descriptor_buffer->m_point_lights.begin() + m_light_stack_index,
								   s_global_light_descriptor_buffer->m_point_lights.end()};

		for (auto& element : subsequent_elements)
			element->m_light_stack_index--;

		s_global_light_descriptor_buffer->m_point_lights.erase(
			s_global_light_descriptor_buffer->m_point_lights.begin() + m_light_stack_index);
	}

	// ===========================================================================

	spot_light::spot_light(const lh::colors::color& color,
						   const light::intensity_t& intensity,
						   const entity::position_t& position,
						   const parameter_precision_t& spread_angle,
						   const parameter_precision_t& sharpness)
		: physical_light {color, intensity, position}, m_spread_angle {spread_angle}, m_sharpness {sharpness}
	{
		s_global_light_descriptor_buffer->m_spot_lights.push_back(this);
		s_global_light_descriptor_buffer->m_spot_light_data.emplace_back(
			glm::vec4 {m_position, 1.0f},
			glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f},
			m_color,
			m_spread_angle,
			m_sharpness);
		m_light_stack_index = s_global_light_descriptor_buffer->m_spot_lights.size() - 1;
		s_global_light_descriptor_buffer->map_spot_lights();
	}

	auto spot_light::spread_angle() const -> const parameter_precision_t&
	{
		return m_spread_angle;
	}

	auto spot_light::spread_angle(const parameter_precision_t& spread_angle) -> void
	{
		m_spread_angle = spread_angle;
		update_light_on_stack();
	}

	auto spot_light::sharpness() const -> const parameter_precision_t&
	{
		return m_sharpness;
	}

	auto spot_light::sharpness(const parameter_precision_t& sharpness) -> void
	{
		m_sharpness = sharpness;
		update_light_on_stack();
	}

	auto spot_light::update_light_on_stack() -> void
	{
		s_global_light_descriptor_buffer->m_spot_light_data[m_light_stack_index] = {
			glm::vec4 {m_position, 1.0f},
			glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f},
			m_color,
			m_spread_angle,
			m_sharpness};

		s_global_light_descriptor_buffer->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f},
						 glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f},
						 m_color,
						 m_spread_angle,
						 m_sharpness},
			s_global_light_descriptor_buffer->create_information().m_point_lights * sizeof point_light::shader_data +
				m_light_stack_index * sizeof shader_data);
	}

	// ===========================================================================

	directional_light::directional_light(const colors::color& color,
										 const light::intensity_t& intensity,
										 const entity::position_t& position,
										 const entity::rotation_t rotation)
		: physical_light {color, intensity, position, rotation}
	{
		s_global_light_descriptor_buffer->m_directional_lights.push_back(this);
		s_global_light_descriptor_buffer->m_directional_light_data.emplace_back(
			glm::vec4 {m_position, 1.0f}, glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f}, m_color);
		m_light_stack_index = s_global_light_descriptor_buffer->m_directional_lights.size() - 1;
		s_global_light_descriptor_buffer->map_directional_lights();
	}

	auto directional_light::update_light_on_stack() -> void
	{
		s_global_light_descriptor_buffer->m_directional_light_data[m_light_stack_index] = {
			glm::vec4 {m_position, 1.0f}, glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f}, m_color};

		s_global_light_descriptor_buffer->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f},
						 glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f},
						 m_color},
			s_global_light_descriptor_buffer->create_information().m_point_lights * sizeof point_light::shader_data +
				s_global_light_descriptor_buffer->create_information().m_spot_lights * sizeof spot_light::shader_data +
				m_light_stack_index * sizeof shader_data);
	}

	// ===========================================================================

	ambient_light::ambient_light(const colors::color& color,
								 const light::intensity_t& intensity,
								 const entity::position_t& position,
								 const parameter_precision_t& decay_factor)
		: physical_light {color, intensity, position}, m_decay_factor {decay_factor}
	{
		s_global_light_descriptor_buffer->m_ambient_lights.push_back(this);
		s_global_light_descriptor_buffer->m_ambient_light_data.emplace_back(glm::vec4 {m_position, 1.0f},
																			m_color,
																			m_decay_factor);
		m_light_stack_index = s_global_light_descriptor_buffer->m_ambient_lights.size() - 1;
		s_global_light_descriptor_buffer->map_ambient_lights();
	}

	auto ambient_light::update_light_on_stack() -> void
	{
		s_global_light_descriptor_buffer->m_ambient_light_data[m_light_stack_index] = {glm::vec4 {m_position, 1.0f},
																					   m_color,
																					   m_decay_factor};

		s_global_light_descriptor_buffer->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f}, m_color, m_decay_factor},
			s_global_light_descriptor_buffer->create_information().m_point_lights * sizeof point_light::shader_data +
				s_global_light_descriptor_buffer->create_information().m_spot_lights * sizeof spot_light::shader_data +
				s_global_light_descriptor_buffer->create_information().m_directional_lights *
					sizeof directional_light::shader_data +
				m_light_stack_index * sizeof shader_data);
	}

	// ===========================================================================

	global_light_descriptor_buffer::global_light_descriptor_buffer(const vulkan::physical_device& physical_device,
																   const vulkan::logical_device& logical_device,
																   const vulkan::memory_allocator& memory_allocator,
																   const create_info& create_info)
		: m_create_info {create_info},
		  m_point_light_data {},
		  m_spot_light_data {},
		  m_directional_light_data {},
		  m_ambient_light_data {},
		  m_light_resource_buffer {}
	{
		const auto& descriptor_buffer_properties = physical_device.properties().m_descriptor_buffer_properties;

		light::s_global_light_descriptor_buffer = this;

		m_point_light_data.reserve(create_info.m_point_lights);
		m_spot_light_data.reserve(create_info.m_spot_lights);
		m_directional_light_data.reserve(create_info.m_directional_lights);
		m_ambient_light_data.reserve(create_info.m_ambient_lights);

		const auto buffer_size = create_info.m_point_lights * sizeof point_light::shader_data +
								 create_info.m_spot_lights * sizeof spot_light::shader_data +
								 create_info.m_directional_lights * sizeof directional_light::shader_data +
								 create_info.m_ambient_lights * sizeof ambient_light::shader_data;

		auto buffer_subdata = decltype(vulkan::descriptor_resource_buffer::create_info::m_subdata) {};

		buffer_subdata.reserve(create_info.m_point_lights + create_info.m_spot_lights +
							   create_info.m_directional_lights + create_info.m_ambient_lights);

		const auto combined_lights_size = create_info.m_point_lights + create_info.m_spot_lights +
										  create_info.m_directional_lights + create_info.m_ambient_lights;

		buffer_subdata.emplace_back(
			vk::DescriptorType::eStorageBuffer,
			vulkan::buffer_subdata::subdata {0, create_info.m_point_lights * sizeof point_light::shader_data});

		buffer_subdata.emplace_back(
			vk::DescriptorType::eStorageBuffer,
			vulkan::buffer_subdata::subdata {create_info.m_point_lights * sizeof point_light::shader_data,
											 create_info.m_spot_lights * sizeof spot_light::shader_data});

		buffer_subdata.emplace_back(
			vk::DescriptorType::eStorageBuffer,
			vulkan::buffer_subdata::subdata {create_info.m_point_lights * sizeof point_light::shader_data +
												 create_info.m_spot_lights * sizeof spot_light::shader_data,
											 create_info.m_directional_lights * sizeof directional_light::shader_data});

		buffer_subdata.emplace_back(vk::DescriptorType::eStorageBuffer,
									vulkan::buffer_subdata::subdata {
										create_info.m_point_lights * sizeof point_light::shader_data +
											create_info.m_spot_lights * sizeof spot_light::shader_data +
											create_info.m_directional_lights * sizeof directional_light::shader_data,
										create_info.m_ambient_lights * sizeof ambient_light::shader_data});

		m_light_resource_buffer = {physical_device,
								   logical_device,
								   memory_allocator,
								   buffer_size,
								   {{.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
												vk::BufferUsageFlagBits::eStorageBuffer},
									buffer_subdata}};
	}

	auto global_light_descriptor_buffer::light_resource_buffer() const -> const vulkan::descriptor_resource_buffer&
	{
		return m_light_resource_buffer;
	}
	/*
	auto global_light_descriptor_buffer::light_storage_bindings() const
		-> const std::vector<vk::DescriptorBufferBindingInfoEXT>&
	{
		return m_light_storage_descriptor_buffer_binding_info;
	}*/

	auto global_light_descriptor_buffer::create_information() const -> const create_info&
	{
		return m_create_info;
	}

	auto global_light_descriptor_buffer::map_point_lights() -> void
	{
		m_light_resource_buffer.mapped_buffer().map_data(*m_point_light_data.data(),
														 0,
														 m_point_light_data.size() * sizeof point_light::shader_data);
	}

	auto global_light_descriptor_buffer::map_spot_lights() -> void
	{
		m_light_resource_buffer.mapped_buffer().map_data(*m_spot_light_data.data(),
														 m_create_info.m_point_lights * sizeof point_light::shader_data,
														 m_spot_light_data.size() * sizeof spot_light::shader_data);
	}

	auto global_light_descriptor_buffer::map_directional_lights() -> void
	{
		m_light_resource_buffer.mapped_buffer().map_data(
			*m_directional_light_data.data(),
			m_create_info.m_point_lights * sizeof point_light::shader_data +
				m_create_info.m_spot_lights * sizeof spot_light::shader_data,
			m_directional_light_data.size() * sizeof directional_light::shader_data);
	}

	auto global_light_descriptor_buffer::map_ambient_lights() -> void
	{
		m_light_resource_buffer.mapped_buffer().map_data(
			*m_ambient_light_data.data(),
			m_create_info.m_point_lights * sizeof point_light::shader_data +
				m_create_info.m_spot_lights * sizeof spot_light::shader_data +
				m_create_info.m_directional_lights * sizeof directional_light::shader_data,
			m_ambient_light_data.size() * sizeof directional_light::shader_data);
	}
}
