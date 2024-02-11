module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <cstdint>
#include <vector>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

export module light;

#if not INTELLISENSE
import std.core;
#endif

import entity;
import color;
import physical_device;
import logical_device;
import memory_allocator;
import buffer;
import descriptor_resource_buffer;
import texture;

export namespace lh
{
	// abstract base light class, defining common light attributes
	// light intensity is encoded into color's alpha channel
	class light
	{
	public:
		using light_stack_size_t = std::uint16_t;
		using global_light_offset_t = std::uint64_t;
		using parameter_precision_t = float;
		using intensity_t = float;

		static inline constexpr auto num_light_types = 4;
		static inline constexpr auto reserved_light_descriptor_set_number = 3;

		light(const light&) = delete;
		light& operator=(const light&) = delete;

		auto color() const -> const colors::color&;
		auto intensity() const -> const intensity_t&;

		friend class global_light_manager;

	protected:
		light(const colors::color&, const intensity_t&);

		colors::color m_color;
		light_stack_size_t m_light_stack_index;

		static inline global_light_manager* s_global_light_manager;
	};

	// abstract base physical light class
	// inherits transformation handling mechanisms from entity and common light properties from light
	// acting as a base for lights with physical positions, orientations and an inverse square intensity decay
	class physical_light : public light, public entity
	{
	public:
		physical_light(const colors::color&,
					   const light::intensity_t&,
					   const entity::position_t& = {},
					   const entity::rotation_t = {},
					   const entity::scale_t = {});

		auto color(const colors::color&) -> void;
		auto intensity(const intensity_t&) -> void;
		auto effective_radius() const -> const light::intensity_t&;
		auto intensity_at(const entity::position_t&) const -> light::intensity_t;

	protected:
		auto on_position_change() -> void override final;
		auto on_rotation_change() -> void override final;
		auto calculate_effective_radius() -> void;

		virtual auto update_light_on_stack() -> void = 0;

		template<typename T>
		auto remove_light_from_stack() -> void;

		light::intensity_t m_effective_radius;
	};

	// specialized physical light, radiates light in all directions
	class point_light final : public physical_light
	{
	public:
		struct shader_data
		{
			glm::vec4 m_position;
			glm::vec4 m_color;
		};

		point_light(const lh::colors::color&, const light::intensity_t&, const entity::position_t&);
		~point_light();

	private:
		auto update_light_on_stack() -> void override final;
	};

	// specialized physical light, radiates light in a cone
	class spot_light final : public physical_light
	{
	public:
		struct shader_data
		{
			glm::vec4 m_position;
			glm::vec4 m_rotation;
			glm::vec4 m_color;
			parameter_precision_t m_spread_angle;
			parameter_precision_t m_sharpness;
			parameter_precision_t m_alignment_padding[2] = {0.0f, 0.0f};
		};

		spot_light(const lh::colors::color&,
				   const light::intensity_t&,
				   const entity::position_t&,
				   const parameter_precision_t& spread_angle = 45.0f,
				   const parameter_precision_t& sharpness = 1.0f);
		~spot_light();

		auto spread_angle() const -> const parameter_precision_t&;
		auto spread_angle(const parameter_precision_t&) -> void;
		auto sharpness() const -> const parameter_precision_t&;
		auto sharpness(const parameter_precision_t&) -> void;

	private:
		auto update_light_on_stack() -> void override final;


		parameter_precision_t m_spread_angle;
		parameter_precision_t m_sharpness;
	};

	// specialized non physical light, radiates parallel light rays with an infinite radius and no decay
	class directional_light final : public physical_light
	{
	public:
		struct shader_data
		{
			glm::vec4 m_position;
			glm::vec4 m_rotation;
			glm::vec4 m_color;
		};

		directional_light(const colors::color&,
						  const light::intensity_t&,
						  const entity::position_t&,
						  const entity::rotation_t);
		~directional_light();

	private:
		auto update_light_on_stack() -> void override final;
	};

	// specialized non physical light, applies linearly decaying lighting in affected radius
	class ambient_light final : public physical_light
	{
	public:
		struct shader_data
		{
			glm::vec4 m_position;
			glm::vec4 m_color;
			parameter_precision_t m_decay_factor = 0.0f;
			parameter_precision_t m_alignment_padding[3] = {0.0f, 0.0f, 0.0f};
		};

		ambient_light(const colors::color&, const light::intensity_t&, const entity::position_t&, const parameter_precision_t& decay_factor = 0.0f);
		~ambient_light();

		auto decay_factor() const -> const parameter_precision_t&;
		auto decay_factor(const parameter_precision_t&) -> void;

	private:
		auto update_light_on_stack() -> void override final;

		parameter_precision_t m_decay_factor;
	};

	// ===========================================================================

	class global_light_manager
	{
	public:
		struct create_info
		{
			light::light_stack_size_t m_point_lights = 8;
			light::light_stack_size_t m_spot_lights = 4;
			light::light_stack_size_t m_directional_lights = 2;
			light::light_stack_size_t m_ambient_lights = 4;
		};

		friend class light;
		friend class physical_light;
		friend class point_light;
		friend class spot_light;
		friend class directional_light;
		friend class ambient_light;

		global_light_manager(const vulkan::physical_device&,
									   const vulkan::logical_device&,
									   const vulkan::memory_allocator&,
									   const create_info& = {});

		auto light_resource_buffer() const -> const vulkan::descriptor_resource_buffer&;
		auto create_information() const -> const create_info&;

		auto point_lights() const -> const std::vector<point_light*>&;
		auto spot_lights() const -> const std::vector<spot_light*>&;
		auto directional_lights() const -> const std::vector<directional_light*>&;
		auto ambient_lights() const -> const std::vector<ambient_light*>&;

		auto light_device_addresses() const -> const std::array<vk::DeviceAddress, 4>&;

	private:
		create_info m_create_info;

		std::vector<point_light*> m_point_lights;
		std::vector<spot_light*> m_spot_lights;
		std::vector<directional_light*> m_directional_lights;
		std::vector<ambient_light*> m_ambient_lights;

		vulkan::descriptor_resource_buffer m_light_resource_buffer;
		std::array<vk::DeviceAddress, 4> m_light_device_addresses;
	};

	template <typename T>
	auto physical_light::remove_light_from_stack() -> void
	{
		auto subsequent_elements = std::ranges::subrange<std::vector<T*>::iterator> {};

		if constexpr (std::is_same_v<T, point_light>)
		{
			s_global_light_manager->m_point_lights.erase(s_global_light_manager->m_point_lights.begin() +
														   m_light_stack_index);
			subsequent_elements = std::ranges::subrange {s_global_light_manager->m_point_lights.begin() +
															 m_light_stack_index,
														 s_global_light_manager->m_point_lights.end()};
		}

		if constexpr (std::is_same_v<T, spot_light>)
		{
			s_global_light_manager->m_spot_lights.erase(s_global_light_manager->m_spot_lights.begin() +
															m_light_stack_index);
			subsequent_elements = std::ranges::subrange {s_global_light_manager->m_spot_lights.begin() +
																		m_light_stack_index,
																	s_global_light_manager->m_spot_lights.end()};
		}

		if constexpr (std::is_same_v<T, directional_light>)
		{
			s_global_light_manager->m_directional_lights.erase(s_global_light_manager->m_directional_lights.begin() +
														   m_light_stack_index);
			subsequent_elements = std::ranges::subrange {s_global_light_manager->m_directional_lights.begin() +
																		m_light_stack_index,
																	s_global_light_manager->m_directional_lights.end()};
		}

		if constexpr (std::is_same_v<T, ambient_light>)
		{
			s_global_light_manager->m_ambient_lights.erase(s_global_light_manager->m_ambient_lights.begin() +
														   m_light_stack_index);
			subsequent_elements = std::ranges::subrange {s_global_light_manager->m_ambient_lights.begin() +
															 m_light_stack_index,
														 s_global_light_manager->m_ambient_lights.end()};
		}

		for (auto& element : subsequent_elements)
			element->m_light_stack_index--;
	}
}

module :private;

namespace lh
{
	// given a physical light's intensity and the inverse square law for light intensity decay,
	// we can calculate the distance past which a physical light's color contribution can be discarded for added performance
	// threshold is an arbitrary value that is considered to no longer provide a significant contribution to final
	// lighting output
	constexpr auto physical_light_distance_threshold = 100.0f;
}