module;

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

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
		s_global_light_descriptor_buffer->m_point_light_data.emplace_back(glm::vec4 {m_position, 1.0f}, m_color);
		m_light_stack_index = s_global_light_descriptor_buffer->m_point_light_data.size() - 1;
		s_global_light_descriptor_buffer->map_point_lights();
	}

	auto point_light::update_light_on_stack() -> void
	{
		s_global_light_descriptor_buffer->m_point_light_data[m_light_stack_index] = {glm::vec4 {m_position, 1.0f},
																					 m_color};
	}

	// ===========================================================================

	spot_light::spot_light(const lh::colors::color& color,
						   const light::intensity_t& intensity,
						   const entity::position_t& position,
						   const parameter_precision_t& spread_angle,
						   const parameter_precision_t& sharpness)
		: physical_light {color, intensity, position}, m_spread_angle {spread_angle}, m_sharpness {sharpness}
	{
		s_global_light_descriptor_buffer->m_spot_light_data.emplace_back(
			glm::vec4 {m_position, 1.0f},
			glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f},
			m_color,
			m_spread_angle,
			m_sharpness);
		m_light_stack_index = s_global_light_descriptor_buffer->m_spot_light_data.size() - 1;
		s_global_light_descriptor_buffer->map_spot_lights();
	}

	auto spot_light::spread_angle() const -> const parameter_precision_t&
	{
		return m_spread_angle;
	}

	auto spot_light::spread_angle(const parameter_precision_t& spread_angle) -> void
	{
		m_spread_angle = spread_angle;
	}

	auto spot_light::sharpness() const -> const parameter_precision_t&
	{
		return m_sharpness;
	}

	auto spot_light::sharpness(const parameter_precision_t& sharpness) -> void
	{
		m_sharpness = sharpness;
	}

	auto spot_light::update_light_on_stack() -> void
	{
		s_global_light_descriptor_buffer->m_spot_light_data[m_light_stack_index] = {
			glm::vec4 {m_position, 1.0f},
			glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f},
			m_color,
			m_spread_angle,
			m_sharpness};
	}

	// ===========================================================================

	directional_light::directional_light(const colors::color& color,
										 const light::intensity_t& intensity,
										 const entity::position_t& position,
										 const entity::rotation_t rotation)
		: physical_light {color, intensity, position, rotation}
	{
		s_global_light_descriptor_buffer->m_directional_light_data.emplace_back(
			glm::vec4 {m_position, 1.0f}, glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f}, m_color);
		m_light_stack_index = s_global_light_descriptor_buffer->m_directional_light_data.size() - 1;
		s_global_light_descriptor_buffer->map_directional_lights();
	}

	auto directional_light::update_light_on_stack() -> void
	{
		s_global_light_descriptor_buffer->m_directional_light_data[m_light_stack_index] = {
			glm::vec4 {m_position, 1.0f}, glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f}, m_color};
	}

	// ===========================================================================

	ambient_light::ambient_light(const colors::color& color,
								 const light::intensity_t& intensity,
								 const entity::position_t& position)
		: physical_light {color, intensity, position}
	{
		s_global_light_descriptor_buffer->m_ambient_light_data.emplace_back(glm::vec4 {m_position, 1.0f}, m_color);
		m_light_stack_index = s_global_light_descriptor_buffer->m_ambient_light_data.size() - 1;
		s_global_light_descriptor_buffer->map_ambient_lights();
	}

	auto ambient_light::update_light_on_stack() -> void
	{
		s_global_light_descriptor_buffer->m_ambient_light_data[m_light_stack_index] = {glm::vec4 {m_position, 1.0f},
																					   m_color};
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

		auto buffer_offset = vk::DeviceSize {};
		/*
		for (auto i = light::light_stack_size_t {}; i < combined_lights_size; i++)
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
		*/
		buffer_subdata.emplace_back(
			vk::DescriptorType::eStorageBuffer,
			vulkan::buffer_subdata::subdata {0, /*create_info.m_point_lights*/ 1 * sizeof point_light::shader_data});
		/*
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
										create_info.m_ambient_lights * sizeof ambient_light::shader_data});*/

		m_light_resource_buffer = {physical_device,
								   logical_device,
								   memory_allocator,
								   256,
								   {{.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
												vk::BufferUsageFlagBits::eStorageBuffer},
									buffer_subdata}};
		/*
		for (auto i = light::light_stack_size_t {}; i < combined_lights_size; i++)
		{
			m_light_storage_descriptor_buffer_binding_info.emplace_back(
				m_light_resource_buffer.mapped_buffer().address() +
					i * vulkan::utility::aligned_size(
							static_cast<vk::DeviceSize>(descriptor_buffer_properties.m_storage_buffer_size),
							descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment),
				vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);
		}
		*/
		/*
		const auto aligned_binding_offset = vulkan::utility::aligned_size(
			static_cast<vk::DeviceSize>(descriptor_buffer_properties.m_storage_buffer_size),
			descriptor_buffer_properties.m_properties.descriptorBufferOffsetAlignment);

		m_light_storage_descriptor_buffer_binding_info.emplace_back(
			m_light_resource_buffer.mapped_buffer().address(),
			vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

		m_light_storage_descriptor_buffer_binding_info.emplace_back(
			m_light_resource_buffer.mapped_buffer().address() + create_info.m_point_lights * aligned_binding_offset,
			vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

		m_light_storage_descriptor_buffer_binding_info.emplace_back(
			m_light_resource_buffer.mapped_buffer().address() + create_info.m_point_lights * aligned_binding_offset +
				create_info.m_spot_lights * aligned_binding_offset,
			vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);

		m_light_storage_descriptor_buffer_binding_info.emplace_back(
			m_light_resource_buffer.mapped_buffer().address() + create_info.m_point_lights * aligned_binding_offset +
				create_info.m_spot_lights * aligned_binding_offset +
				create_info.m_directional_lights * aligned_binding_offset,
			vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);*/
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
