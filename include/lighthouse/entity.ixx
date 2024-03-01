module;

#if INTELLISENSE
#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"

#include <memory>
#endif

export module entity;

#if not INTELLISENSE
import glm;
import std.core;
#endif

import node;
import window;
import lighthouse_utility;
import geometry;

export namespace lh
{
	class entity
	{
	public:
		entity(std::shared_ptr<node> = std::make_shared<node>());
		entity(const geometry::position_t&, const geometry::rotation_t = {}, const geometry::scale_t& = {});

		auto position() const -> const geometry::position_t&;
		auto rotation() const -> const geometry::rotation_t;
		auto orientation() const -> const geometry::orientation_t&;
		auto scale() const -> const geometry::scale_t&;

		auto translate_relative(const geometry::position_t&) -> void;
		auto translate_relative(const geometry::rotation_t&, geometry::scalar_t) -> void;
		auto rotate_relative(const geometry::rotation_t&) -> void;
		auto rotate_relative(const geometry::orientation_t&) -> void;
		auto scale_relative(const geometry::scale_t&) -> void;

		auto translate_absolute(const geometry::position_t&) -> void;
		auto rotate_absolute(const geometry::rotation_t&) -> void;
		auto rotate_absolute(const geometry::orientation_t&) -> void;
		auto scale_absolute(const geometry::scale_t&) -> void;

		auto local_transformation(const geometry::transformation_t&) -> void;
		auto local_transformation() const -> const geometry::transformation_t&;
		auto global_transformation() const -> const geometry::transformation_t;

	protected:
		virtual auto on_position_change() -> void {};
		virtual auto on_rotation_change() -> void {};
		virtual auto on_scale_change() -> void {};

		auto reconstruct_node() const -> void;

		geometry::position_t m_position;
		geometry::orientation_t m_orientation;
		geometry::scale_t m_scale;

		mutable bool m_node_requires_reconstruction;
		std::shared_ptr<lh::node> m_node;
	};
}