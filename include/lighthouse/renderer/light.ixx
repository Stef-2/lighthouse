module;

#if INTELLISENSE
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
		using intensity_t = float;

		light(const colors::color&, const intensity_t&);

		auto color() const -> const colors::color&;
		auto color(const colors::color&) -> void;
		auto intensity() const -> const intensity_t&;
		virtual auto intensity(const intensity_t&) -> void;

		friend class global_light_descriptor_buffer;

	protected:
		virtual auto register_light() -> void = 0;

		colors::color m_color;
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

		auto intensity(const intensity_t&) -> void override final;
		auto effective_radius() const -> const light::intensity_t&;
		auto intensity_at(const entity::position_t&) const -> light::intensity_t;

	protected:
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
		auto register_light() -> void override final;
	};

	// specialized physical light, radiates light in a cone
	class spot_light final : public physical_light
	{
	public:
		using parameter_precision_t = float;

		struct shader_data
		{
			glm::vec4 m_position;
			glm::vec4 m_color;
			parameter_precision_t m_spread_angle;
			parameter_precision_t m_sharpness;
			float m_alignment_padding[2] = {0.0f, 0.0f};
		};

	private:
		parameter_precision_t m_spread_angle;
		parameter_precision_t m_sharpness;
	};

	// specialized non physical light, radiates parallel light rays with an infinite radius and no decay
	class directional_light final : public light
	{
	public:
		struct shader_data
		{
			glm::vec4 m_position;
			glm::vec4 m_rotation;
			glm::vec4 m_color;
		};

	private:
		entity::position_t m_position;
		entity::rotation_t m_rotation;
	};

	// specialized non physical light, applies linearly decaying lighting in affected radius
	class ambient_light final : public light
	{
	public:
		struct shader_data
		{
			glm::vec4 m_position;
			glm::vec4 m_color;
		};

	private:
		entity::position_t m_position;
	};

	// ===========================================================================

	class global_light_descriptor_buffer
	{
	public:
		struct create_info
		{
			using light_size_t = std::uint16_t;

			light_size_t m_point_lights = 1024;
			light_size_t m_spot_lights = 512;
			light_size_t m_directional_lights = 32;
			light_size_t m_ambient_lights = 512;
		};

		struct light_info_data
		{
			std::uint32_t m_num_active_point_lights = {};
			std::vector<std::uint32_t> m_active_spot_lights = {};

			std::uint32_t m_num_active_spot_lights = {};
			std::vector<std::uint32_t> m_active_directional_lights = {};

			std::uint32_t m_num_active_directional_lights = {};
			std::vector<std::uint32_t> m_active_point_lights = {};

			std::uint32_t m_num_active_ambient_lights = {};
			std::vector<std::uint32_t> m_active_ambient_lights = {};
		};

		friend class point_light;

		global_light_descriptor_buffer(const vulkan::physical_device&,
									   const vulkan::logical_device&,
									   const vulkan::memory_allocator&,
									   const create_info& = {});

	private:
		create_info m_create_info;
		light_info_data m_light_info_data;
		std::vector<point_light::shader_data> m_point_light_data;
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