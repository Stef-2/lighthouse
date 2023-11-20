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
		update_light_on_stack();
	}

	auto physical_light::on_rotation_change() -> void
	{
		update_light_on_stack();
	}

	// ===========================================================================

	point_light::point_light(const lh::colors::color& color,
							 const light::intensity_t& intensity,
							 const entity::position_t& position)
		: physical_light {color, intensity, position}
	{
		s_global_light_manager->m_point_lights.push_back(this);
		m_light_stack_index = s_global_light_manager->m_point_lights.size() - 1;
		update_light_on_stack();
	}

	point_light::~point_light()
	{
		remove_light_from_stack();
	}

	auto point_light::global_light_buffer_offset() -> global_light_offset_t
	{
		return m_light_stack_index * sizeof shader_data;
	}

	auto point_light::update_light_on_stack() -> void
	{
		s_global_light_manager->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f}, m_color}, global_light_buffer_offset());
	}

	auto point_light::remove_light_from_stack() -> void
	{
		const auto subsequent_elements = std::ranges::subrange {s_global_light_manager->m_point_lights.begin() +
																	m_light_stack_index,
																s_global_light_manager->m_point_lights.end()};

		for (auto& element : subsequent_elements)
			element->m_light_stack_index--;
		rlos<point_light>();
		auto this_light_data =
			static_cast<shader_data*>(
				s_global_light_manager->light_resource_buffer().mapped_buffer().mapped_data_pointer()) +
			global_light_buffer_offset();

		auto last_light_data =
			static_cast<shader_data*>(
				s_global_light_manager->light_resource_buffer().mapped_buffer().mapped_data_pointer()) +
			s_global_light_manager->m_point_lights.size() * sizeof shader_data;

		s_global_light_manager->m_point_lights.erase(s_global_light_manager->m_point_lights.begin() +
													 m_light_stack_index);

		if (not s_global_light_manager->m_point_lights.empty())
			;
	}

	// ===========================================================================

	spot_light::spot_light(const lh::colors::color& color,
						   const light::intensity_t& intensity,
						   const entity::position_t& position,
						   const parameter_precision_t& spread_angle,
						   const parameter_precision_t& sharpness)
		: physical_light {color, intensity, position}, m_spread_angle {spread_angle}, m_sharpness {sharpness}
	{
		s_global_light_manager->m_spot_lights.push_back(this);
		m_light_stack_index = s_global_light_manager->m_spot_lights.size() - 1;
		update_light_on_stack();
	}

	spot_light::~spot_light()
	{
		remove_light_from_stack();
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

	auto spot_light::global_light_buffer_offset() -> global_light_offset_t
	{
		return s_global_light_manager->create_information().m_point_lights * sizeof point_light::shader_data +
			   m_light_stack_index * sizeof shader_data;
	}

	auto spot_light::update_light_on_stack() -> void
	{
		s_global_light_manager->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f},
						 glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f},
						 m_color,
						 m_spread_angle,
						 m_sharpness},
			global_light_buffer_offset());
	}

	auto spot_light::remove_light_from_stack() -> void
	{
		const auto subsequent_elements = std::ranges::subrange {s_global_light_manager->m_spot_lights.begin() +
																	m_light_stack_index,
																s_global_light_manager->m_spot_lights.end()};

		for (auto& element : subsequent_elements)
			element->m_light_stack_index--;

		s_global_light_manager->m_spot_lights.erase(s_global_light_manager->m_spot_lights.begin() +
													m_light_stack_index);
	}

	// ===========================================================================

	directional_light::directional_light(const colors::color& color,
										 const light::intensity_t& intensity,
										 const entity::position_t& position,
										 const entity::rotation_t rotation)
		: physical_light {color, intensity, position, rotation}
	{
		s_global_light_manager->m_directional_lights.push_back(this);
		m_light_stack_index = s_global_light_manager->m_directional_lights.size() - 1;
		update_light_on_stack();
	}

	directional_light::~directional_light()
	{
		remove_light_from_stack();
	}

	auto directional_light::global_light_buffer_offset() -> global_light_offset_t
	{
		return s_global_light_manager->create_information().m_point_lights * sizeof point_light::shader_data +
			   s_global_light_manager->create_information().m_spot_lights * sizeof spot_light::shader_data +
			   m_light_stack_index * sizeof shader_data;
	}

	auto directional_light::update_light_on_stack() -> void
	{
		s_global_light_manager->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f},
						 glm::vec4 {glm::degrees(glm::eulerAngles(m_rotation)), 1.0f},
						 m_color},
			global_light_buffer_offset());
	}

	auto directional_light::remove_light_from_stack() -> void
	{
		const auto subsequent_elements = std::ranges::subrange {s_global_light_manager->m_directional_lights.begin() +
																	m_light_stack_index,
																s_global_light_manager->m_directional_lights.end()};

		for (auto& element : subsequent_elements)
			element->m_light_stack_index--;

		s_global_light_manager->m_directional_lights.erase(s_global_light_manager->m_directional_lights.begin() +
														   m_light_stack_index);
	}

	// ===========================================================================

	ambient_light::ambient_light(const colors::color& color,
								 const light::intensity_t& intensity,
								 const entity::position_t& position,
								 const parameter_precision_t& decay_factor)
		: physical_light {color, intensity, position}, m_decay_factor {decay_factor}
	{
		s_global_light_manager->m_ambient_lights.push_back(this);
		m_light_stack_index = s_global_light_manager->m_ambient_lights.size() - 1;
	}

	ambient_light::~ambient_light()
	{
		remove_light_from_stack();
	}

	auto ambient_light::decay_factor() const -> const parameter_precision_t&
	{
		return m_decay_factor;
	}

	auto ambient_light::decay_factor(const parameter_precision_t& decay_factor) -> void
	{
		m_decay_factor = decay_factor;
		update_light_on_stack();
	}

	auto ambient_light::global_light_buffer_offset() -> global_light_offset_t
	{
		return s_global_light_manager->create_information().m_point_lights * sizeof point_light::shader_data +
			   s_global_light_manager->create_information().m_spot_lights * sizeof spot_light::shader_data +
			   s_global_light_manager->create_information().m_directional_lights *
				   sizeof directional_light::shader_data +
			   m_light_stack_index * sizeof shader_data;
	}

	auto ambient_light::update_light_on_stack() -> void
	{
		s_global_light_manager->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f}, m_color, m_decay_factor}, global_light_buffer_offset());
	}

	auto ambient_light::remove_light_from_stack() -> void
	{
		const auto subsequent_elements = std::ranges::subrange {s_global_light_manager->m_ambient_lights.begin() +
																	m_light_stack_index,
																s_global_light_manager->m_ambient_lights.end()};

		for (auto& element : subsequent_elements)
			element->m_light_stack_index--;

		s_global_light_manager->m_ambient_lights.erase(s_global_light_manager->m_ambient_lights.begin() +
													   m_light_stack_index);
	}

	// ===========================================================================

	global_light_manager::global_light_manager(const vulkan::physical_device& physical_device,
											   const vulkan::logical_device& logical_device,
											   const vulkan::memory_allocator& memory_allocator,
											   const create_info& create_info)
		: m_create_info {create_info},
		  m_point_lights {},
		  m_spot_lights {},
		  m_directional_lights {},
		  m_ambient_lights {},
		  m_light_resource_buffer {}
	{
		const auto& descriptor_buffer_properties = physical_device.properties().m_descriptor_buffer_properties;

		light::s_global_light_manager = this;

		m_point_lights.reserve(create_info.m_point_lights);
		m_spot_lights.reserve(create_info.m_spot_lights);
		m_directional_lights.reserve(create_info.m_directional_lights);
		m_ambient_lights.reserve(create_info.m_ambient_lights);

		const auto point_light_buffer_size = create_info.m_point_lights * sizeof point_light::shader_data;
		const auto spot_light_buffer_size = create_info.m_spot_lights * sizeof spot_light::shader_data;
		const auto directional_light_buffer_size = create_info.m_directional_lights *
												   sizeof directional_light::shader_data;
		const auto ambient_light_buffer_size = create_info.m_ambient_lights * sizeof ambient_light::shader_data;

		const auto combined_buffer_size = point_light_buffer_size + spot_light_buffer_size +
										  directional_light_buffer_size + ambient_light_buffer_size;

		auto buffer_subdata = decltype(vulkan::descriptor_resource_buffer::create_info::m_subdata) {};

		buffer_subdata.reserve(static_cast<std::size_t>(create_info.m_point_lights) + create_info.m_spot_lights +
							   create_info.m_directional_lights + create_info.m_ambient_lights);

		const auto combined_lights_size = create_info.m_point_lights + create_info.m_spot_lights +
										  create_info.m_directional_lights + create_info.m_ambient_lights;

		buffer_subdata.emplace_back(vk::DescriptorType::eStorageBuffer,
									vulkan::buffer_subdata::subdata {0, point_light_buffer_size});

		buffer_subdata.emplace_back(vk::DescriptorType::eStorageBuffer,
									vulkan::buffer_subdata::subdata {point_light_buffer_size, spot_light_buffer_size});

		buffer_subdata.emplace_back(vk::DescriptorType::eStorageBuffer,
									vulkan::buffer_subdata::subdata {point_light_buffer_size + spot_light_buffer_size,
																	 directional_light_buffer_size});

		buffer_subdata.emplace_back(vk::DescriptorType::eStorageBuffer,
									vulkan::buffer_subdata::subdata {point_light_buffer_size + spot_light_buffer_size +
																		 directional_light_buffer_size,
																	 ambient_light_buffer_size});

		m_light_resource_buffer = {physical_device,
								   logical_device,
								   memory_allocator,
								   combined_buffer_size,
								   {{.m_usage = vk::BufferUsageFlagBits::eShaderDeviceAddress |
												vk::BufferUsageFlagBits::eStorageBuffer},
									buffer_subdata}};
	}

	auto global_light_manager::light_resource_buffer() const -> const vulkan::descriptor_resource_buffer&
	{
		return m_light_resource_buffer;
	}

	auto global_light_manager::create_information() const -> const create_info&
	{
		return m_create_info;
	}
}
