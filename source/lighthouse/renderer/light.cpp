module;

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

	// ===========================================================================

	physical_light::physical_light(const colors::color& color,
								   const light::intensity_t& intensity,
								   const geometry::position_t& position,
								   const geometry::normal_t rotation,
								   const geometry::scale_t scale)
		: light {color, intensity}, entity {position, rotation, scale}
	{
		calculate_effective_radius();
	}

	auto physical_light::color(const colors::color& color) -> void
	{
		m_color = {color.r, color.g, color.b, m_color.a};
		m_effective_radius = glm::sqrt(m_color.a / physical_light_distance_threshold);

		update_light_on_stack();
	}

	auto physical_light::intensity(const intensity_t& intensity) -> void
	{
		m_color.a = intensity;
		update_light_on_stack();

		// given the inverse square intensity decay law, calculate the distance past which
		// this lights contribution can be discarded
		// threshold is an arbitrary value that is considered to no longer provide a significant contribution to final
		// lighting output
		calculate_effective_radius();
	}

	auto physical_light::effective_radius() const -> const light::intensity_t&
	{
		return m_effective_radius;
	}

	auto physical_light::intensity_at(const geometry::position_t& position) const -> light::intensity_t
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

	auto physical_light::calculate_effective_radius() -> void
	{
		m_effective_radius = glm::sqrt(m_color.a / physical_light_distance_threshold);
	}

	// ===========================================================================

	point_light::point_light(const lh::colors::color& color,
							 const light::intensity_t& intensity,
							 const geometry::position_t& position)
		: physical_light {color, intensity, position}
	{
		s_global_light_manager->m_point_lights.push_back(this);
		m_light_stack_index = s_global_light_manager->m_point_lights.size() - 1;
		update_light_on_stack();
	}

	point_light::~point_light()
	{
		remove_light_from_stack<point_light>();
	}

	auto point_light::update_light_on_stack() -> void
	{
		s_global_light_manager->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f}, m_color}, m_light_stack_index * sizeof shader_data);
	}

	// ===========================================================================

	spot_light::spot_light(const lh::colors::color& color,
						   const light::intensity_t& intensity,
						   const geometry::position_t& position,
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
		remove_light_from_stack<spot_light>();
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
		s_global_light_manager->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f},
						 glm::vec4 {glm::degrees(glm::eulerAngles(m_orientation)), 1.0f},
						 m_color,
						 m_spread_angle,
						 m_sharpness},
			s_global_light_manager->create_information().m_point_lights * sizeof point_light::shader_data +
				m_light_stack_index * sizeof shader_data);
	}

	// ===========================================================================

	directional_light::directional_light(const colors::color& color,
										 const light::intensity_t& intensity,
										 const geometry::position_t& position,
										 const geometry::normal_t rotation)
		: physical_light {color, intensity, position, rotation}
	{
		s_global_light_manager->m_directional_lights.push_back(this);
		m_light_stack_index = s_global_light_manager->m_directional_lights.size() - 1;
		update_light_on_stack();
	}

	directional_light::~directional_light()
	{
		remove_light_from_stack<directional_light>();
	}

	auto directional_light::update_light_on_stack() -> void
	{
		s_global_light_manager->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f},
						 glm::vec4 {glm::degrees(glm::eulerAngles(m_orientation)), 1.0f},
						 m_color},
			s_global_light_manager->create_information().m_point_lights * sizeof point_light::shader_data +
				s_global_light_manager->create_information().m_spot_lights * sizeof spot_light::shader_data +
				m_light_stack_index * sizeof shader_data);
	}

	// ===========================================================================

	ambient_light::ambient_light(const colors::color& color,
								 const light::intensity_t& intensity,
								 const geometry::position_t& position,
								 const parameter_precision_t& decay_factor)
		: physical_light {color, intensity, position}, m_decay_factor {decay_factor}
	{
		s_global_light_manager->m_ambient_lights.push_back(this);
		m_light_stack_index = s_global_light_manager->m_ambient_lights.size() - 1;
		update_light_on_stack();
	}

	ambient_light::~ambient_light()
	{
		remove_light_from_stack<ambient_light>();
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

	auto ambient_light::update_light_on_stack() -> void
	{
		s_global_light_manager->light_resource_buffer().mapped_buffer().map_data(
			shader_data {glm::vec4 {m_position, 1.0f}, m_color, m_decay_factor},
			s_global_light_manager->create_information().m_point_lights * sizeof point_light::shader_data +
				s_global_light_manager->create_information().m_spot_lights * sizeof spot_light::shader_data +
				s_global_light_manager->create_information().m_directional_lights *
					sizeof directional_light::shader_data +
				m_light_stack_index * sizeof shader_data);
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
		  m_light_resource_buffer {},
		  m_light_device_addresses {}
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
		const auto& buffer_address = m_light_resource_buffer.mapped_buffer().address();

		m_light_device_addresses = {buffer_address,
									buffer_address + point_light_buffer_size,
									buffer_address + point_light_buffer_size + spot_light_buffer_size,
									buffer_address + point_light_buffer_size + spot_light_buffer_size +
										directional_light_buffer_size};
	}

	auto global_light_manager::light_resource_buffer() const -> const vulkan::descriptor_resource_buffer&
	{
		return m_light_resource_buffer;
	}

	auto global_light_manager::create_information() const -> const create_info&
	{
		return m_create_info;
	}

	auto global_light_manager::point_lights() const -> const std::vector<point_light*>&
	{
		return m_point_lights;
	}

	auto global_light_manager::spot_lights() const -> const std::vector<spot_light*>&
	{
		return m_spot_lights;
	}

	auto global_light_manager::directional_lights() const -> const std::vector<directional_light*>&
	{
		return m_directional_lights;
	}

	auto global_light_manager::ambient_lights() const -> const std::vector<ambient_light*>&
	{
		return m_ambient_lights;
	}
	auto global_light_manager::light_device_addresses() const -> const std::array<vk::DeviceAddress, 4>&
	{
		return m_light_device_addresses;
	}
}
