module;

#include "glm/vec3.hpp"
#include "glm/gtx/quaternion.hpp"

#if INTELLISENSE
#include <memory>
#endif

export module entity;

import node;
import window;
import lh_utility;

export namespace lh
{
	class entity
	{
	public:
		using transformation_t = node::transformation_t;
		using normalized_direction_t = glm::vec3;
		using vector3_t = glm::vec3;
		using rotation_t = glm::quat;

		entity(std::shared_ptr<node> = std::make_shared<node>());

		auto translate_relative(const vector3_t&) -> void;
		auto translate_relative(const normalized_direction_t&, float) -> void;
		auto rotate_relative(const vector3_t&) -> void;
		auto rotate_relative(const rotation_t&) -> void;
		auto scale_relative(const vector3_t&) -> void;

		auto translate_absolute(const vector3_t&) -> void;
		auto rotate_absolute(const vector3_t&) -> void;
		auto rotate_absolute(const rotation_t&) -> void;
		auto scale_absolute(const vector3_t&) -> void;

		auto local_transformation(const transformation_t&) -> void;
		auto local_transformation() const -> const transformation_t&;
		auto global_transformation() const -> const transformation_t&;

		auto position() const -> const vector3_t& { return m_position; }
		auto rotation() const -> const rotation_t& { return m_rotation; }
		auto scale() const -> const vector3_t& { return m_scale; }

	protected:
		auto reconstruct_node() const -> void;

		vector3_t m_position;
		rotation_t m_rotation;
		vector3_t m_scale;

		mutable bool m_node_requires_reconstruction;
		std::shared_ptr<lh::node> m_node;
	};

}