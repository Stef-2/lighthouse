module;
#pragma once

#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"

export module entity;

#if INTELLISENSE
#include "lighthouse/node.ixx"
#include "lighthouse/utility.ixx"
#else
import node;
import window;
import lh_utility;
#endif

export namespace lh
{
	class entity
	{
	public:
		using transformation_t = node::transformation_t;
		using normalized_direction_t = glm::vec3;

		entity(const node& = {});

		auto translate_relative(const glm::vec3&) -> void;
		auto translate_relative(const normalized_direction_t&, float) -> void;
		auto rotate_relative(const glm::vec3&) -> void;
		auto rotate_relative(const glm::quat&) -> void;
		auto scale_relative(const glm::vec3&) -> void;

		auto translate_absolute(const glm::vec3&) -> void;
		auto rotate_absolute(const glm::vec3&) -> void;
		auto rotate_absolute(const glm::quat&) -> void;
		auto scale_absolute(const glm::vec3&) -> void;

		auto local_transformation(const transformation_t&) -> void;
		auto local_transformation() const -> const transformation_t&;
		auto global_transformation() const -> const transformation_t&;

		auto position() -> glm::vec3& { return m_position; }
		auto rotation() -> glm::quat& { return m_rotation; }
		auto scale() -> glm::vec3& { return m_scale; }

	protected:
		auto reconstruct_node() const -> void;

		glm::vec3 m_position;
		glm::quat m_rotation;
		glm::vec3 m_scale;

		mutable bool m_node_requires_reconstruction;
		non_owning_ptr<lh::node> m_node;
	};

}