module;

#include "glm/gtx/matrix_decompose.hpp"

module entity;

namespace lh
{
	entity::entity(std::shared_ptr<node> node)
		: m_node {node},
		  m_position {},
		  m_rotation {1.0f, 0.0f, 0.0f, 0.0f},
		  m_scale {1.0f, 1.0f, 1.0f},
		  m_node_requires_reconstruction {false}
	{
		// if node transformation is not an identity transformation
		// it needs to be decomposed into components
		if (node->local_transformation() != node::transformation_t {1.0f})
		{
			auto skew = vector3_t {};
			auto perspective = glm::vec4 {};

			glm::decompose(node->local_transformation(), m_scale, m_rotation, m_position, skew, perspective);
		}
	}

	auto entity::translate_relative(const vector3_t& translation) -> void
	{
		m_position += translation;
		m_node_requires_reconstruction = true;
	}

	auto entity::translate_relative(const entity::normalized_direction_t& direction, float magnitude) -> void
	{
		m_position += direction * magnitude;
		m_node_requires_reconstruction = true;
	}

	auto entity::rotate_relative(const vector3_t& rotation) -> void
	{
		m_rotation = m_rotation * rotation_t {rotation};
		m_node_requires_reconstruction = true;
	}
	auto entity::rotate_relative(const rotation_t& rotation) -> void
	{
		m_rotation = m_rotation * rotation;
		m_node_requires_reconstruction = true;
	}

	auto entity::scale_relative(const vector3_t& scaling) -> void
	{
		m_scale += scaling;
		m_node_requires_reconstruction = true;
	}

	auto entity::translate_absolute(const vector3_t& translation) -> void
	{
		m_position = translation;
		m_node_requires_reconstruction = true;
	}

	auto entity::rotate_absolute(const vector3_t& rotation) -> void
	{
		m_rotation = rotation_t {rotation};
	}

	auto entity::rotate_absolute(const rotation_t& rotation) -> void
	{
		m_rotation = rotation;
		m_node_requires_reconstruction = true;
	}

	auto entity::scale_absolute(const vector3_t& scaling) -> void
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
		auto transformation = node::transformation_t {1.0f};

		const auto translate = glm::translate(transformation, m_position);
		const auto rotate = glm::mat4_cast(m_rotation);
		const auto scale = glm::scale(transformation, m_scale);

		m_node->local_transformation(scale * rotate * translate);

		m_node_requires_reconstruction = false;
	}
}
