module;

#include "glm/gtx/matrix_decompose.hpp"

#if INTELLISENSE
#include "lighthouse/entity.ixx"
#else
module entity;
#endif

namespace lh
{
	entity::entity(const lh::node& node)
		: m_node {&const_cast<lh::node&>(node)},
		  m_position {},
		  m_rotation {},
		  m_scale {},
		  m_node_requires_reconstruction {false}
	{
		// if node transformation is not an identity transformation
		// it needs to be decomposed into components
		if (node.local_transformation() != node::s_identity_transformation)
		{
			auto skew = glm::vec3 {};
			auto perspective = glm::vec4 {};

			glm::decompose(node.local_transformation(), m_scale, m_rotation, m_position, skew, perspective);
		}
	}

	auto entity::translate_relative(const glm::vec3& translation) -> void
	{
		m_position += translation;
		m_node_requires_reconstruction = true;
	}

	auto entity::rotate_relative(const glm::vec3& rotation) -> void
	{
		m_rotation += glm::quat {rotation};
		m_node_requires_reconstruction = true;
	}
	auto entity::rotate_relative(const glm::quat& rotation) -> void
	{
		m_rotation += rotation;
		m_node_requires_reconstruction = true;
	}

	auto entity::scale_relative(const glm::vec3& scaling) -> void
	{
		m_scale += scaling;
		m_node_requires_reconstruction = true;
	}

	auto entity::translate_absolute(const glm::vec3& translation) -> void
	{
		m_position = translation;
		m_node_requires_reconstruction = true;
	}

	auto entity::rotate_absolute(const glm::vec3& rotation) -> void
	{
		m_rotation = glm::quat {rotation};
	}

	auto entity::rotate_absolute(const glm::quat& rotation) -> void
	{
		m_rotation = rotation;
		m_node_requires_reconstruction = true;
	}

	auto entity::scale_absolute(const glm::vec3& scaling) -> void
	{
		m_scale = scaling;
		m_node_requires_reconstruction = true;
	}

	auto entity::local_transformation(const transformation_t& transformation) -> void
	{
		m_node->local_transformation(transformation);
	}

	auto entity::local_transformation() const -> const transformation_t&
	{
		if (m_node_requires_reconstruction)
			reconstruct_node();

		return m_node->local_transformation();
	}

	auto entity::global_transformation() const -> const transformation_t&
	{
		return m_node->global_transformation();
	}

	auto entity::reconstruct_node() const -> void
	{
		auto transformation = node::s_identity_transformation;

		transformation = glm::scale(transformation, m_scale);
		transformation *= glm::mat4_cast(m_rotation);
		transformation = glm::translate(transformation, m_position);

		m_node->local_transformation(transformation);

		m_node_requires_reconstruction = false;
	}
}
