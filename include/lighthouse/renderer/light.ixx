module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"

#include <cstdint>
#include <vector>
#endif

#include "glm/vec4.hpp"

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

export namespace lh
{
	// abstract base light class, defining common light attributes
	// light intensity is encoded into color's alpha channel
	class light
	{
	public:
		using light_stack_size_t = std::uint16_t;
		using parameter_precision_t = float;
		using intensity_t = float;

		static inline constexpr auto num_light_types = 4;
		static inline constexpr auto reserved_light_descriptor_set_number = 3;

		light(const colors::color&, const intensity_t&);
		light(const light&) = delete;
		light& operator=(const light&) = delete;

		auto color() const -> const colors::color&;
		auto intensity() const -> const intensity_t&;
		virtual auto intensity(const intensity_t&) -> void;

		friend class global_light_descriptor_buffer;

	protected:
		colors::color m_color;
		light_stack_size_t m_light_stack_index;

		static inline global_light_descriptor_buffer* s_global_light_descriptor_buffer;
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
		auto intensity(const intensity_t&) -> void override final;
		auto effective_radius() const -> const light::intensity_t&;
		auto intensity_at(const entity::position_t&) const -> light::intensity_t;

	protected:
		auto on_position_change() -> void override final;
		auto on_rotation_change() -> void override final;
		virtual auto update_light_on_stack() -> void = 0;
		virtual auto remove_light_from_stack() -> void = 0;

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

	private:
		auto update_light_on_stack() -> void override final;
		auto remove_light_from_stack() -> void override final;
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

	private:
		auto update_light_on_stack() -> void override final;

		parameter_precision_t m_decay_factor;
	};

	// ===========================================================================

	class global_light_descriptor_buffer
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

		global_light_descriptor_buffer(const vulkan::physical_device&,
									   const vulkan::logical_device&,
									   const vulkan::memory_allocator&,
									   const create_info& = {});

		auto light_resource_buffer() const -> const vulkan::descriptor_resource_buffer&;
		auto create_information() const -> const create_info&;

	private:
		auto map_point_lights() -> void;
		auto map_spot_lights() -> void;
		auto map_directional_lights() -> void;
		auto map_ambient_lights() -> void;

		create_info m_create_info;

		std::vector<point_light*> m_point_lights;
		std::vector<spot_light*> m_spot_lights;
		std::vector<directional_light*> m_directional_lights;
		std::vector<ambient_light*> m_ambient_lights;

		std::vector<point_light::shader_data> m_point_light_data;
		std::vector<spot_light::shader_data> m_spot_light_data;
		std::vector<directional_light::shader_data> m_directional_light_data;
		std::vector<ambient_light::shader_data> m_ambient_light_data;

		vulkan::descriptor_resource_buffer m_light_resource_buffer;
	};
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