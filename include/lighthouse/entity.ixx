module;

#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"

#if INTELLISENSE
#include <memory>
#endif

export module entity;

#if not INTELLISENSE
import std.core;
#endif

import node;
import window;
import lighthouse_utility;

export namespace lh
{
	class entity
	{
	public:
		using normalized_direction_t = glm::vec3;
		using position_t = glm::vec3;
		using rotation_t = glm::vec3;
		using scale_t = glm::vec3;
		using orientation_t = glm::quat;

		entity(std::shared_ptr<node> = std::make_shared<node>());

		auto translate_relative(const position_t&) -> void;
		auto translate_relative(const normalized_direction_t&, float) -> void;
		auto rotate_relative(const rotation_t&) -> void;
		auto rotate_relative(const orientation_t&) -> void;
		auto scale_relative(const scale_t&) -> void;

		auto translate_absolute(const position_t&) -> void;
		auto rotate_absolute(const rotation_t&) -> void;
		auto rotate_absolute(const orientation_t&) -> void;
		auto scale_absolute(const scale_t&) -> void;

		auto local_transformation(const node::transformation_t&) -> void;
		auto local_transformation() const -> const node::transformation_t&;
		auto global_transformation() const -> const node::transformation_t&;

		auto position() const -> const position_t& { return m_position; }
		auto rotation() const -> const orientation_t& { return m_rotation; }
		auto scale() const -> const scale_t& { return m_scale; }

	protected:
		auto reconstruct_node() const -> void;

		position_t m_position;
		orientation_t m_rotation;
		scale_t m_scale;

		mutable bool m_node_requires_reconstruction;
		std::shared_ptr<lh::node> m_node;
	};

}