module;

module entity;

namespace lh
{
	entity::entity(std::shared_ptr<node> node)
		: m_node {node},
		  m_position {},
		  m_orientation {1.0f, 0.0f, 0.0f, 0.0f},
		  m_scale {1.0f, 1.0f, 1.0f},
		  m_node_requires_reconstruction {false}
	{
		// if node transformation is not an identity transformation
		// it needs to be decomposed into components
		if (node->local_transformation() != geometry::transformation_t {1.0f})
		{
			auto skew = glm::vec3 {};
			auto perspective = glm::vec4 {};

			glm::decompose(node->local_transformation(), m_scale, m_orientation, m_position, skew, perspective);
		}
	}

	entity::entity(const geometry::position_t& position,
				   const geometry::rotation_t rotation,
				   const geometry::scale_t& scale)
		: m_position {position},
		  m_orientation {rotation},
		  m_scale {scale},
		  m_node_requires_reconstruction {true},
		  m_node {std::make_shared<node>()}
	{}

	auto entity::position() const -> const geometry::position_t&
	{
		return m_position;
	}

	auto entity::rotation() const -> const geometry::rotation_t
	{
		return glm::eulerAngles(m_orientation);
	}

	auto entity::orientation() const -> const geometry::orientation_t&
	{
		return m_orientation;
	}

	auto entity::scale() const -> const geometry::scale_t&
	{
		return m_scale;
	}

	auto entity::translate_relative(const geometry::position_t& translation) -> void
	{
		m_position += translation;
		m_node_requires_reconstruction = true;

		on_position_change();
	}

	auto entity::translate_relative(const geometry::direction_t& direction, geometry::scalar_t magnitude) -> void
	{
		m_position += direction * magnitude;
		m_node_requires_reconstruction = true;

		on_position_change();
	}

	auto entity::rotate_relative(const geometry::rotation_t& rotation) -> void
	{
		m_orientation = m_orientation * geometry::rotation_t {rotation};
		m_node_requires_reconstruction = true;

		on_rotation_change();
	}
	auto entity::rotate_relative(const geometry::orientation_t& rotation) -> void
	{
		m_orientation = m_orientation * rotation;
		m_node_requires_reconstruction = true;
		on_rotation_change();
	}

	auto entity::scale_relative(const geometry::scale_t& scaling) -> void
	{
		m_scale += scaling;
		m_node_requires_reconstruction = true;

		on_scale_change();
	}

	auto entity::translate_absolute(const geometry::position_t& translation) -> void
	{
		m_position = translation;
		m_node_requires_reconstruction = true;

		on_position_change();
	}

	auto entity::rotate_absolute(const geometry::rotation_t& rotation) -> void
	{
		m_orientation = geometry::rotation_t {rotation};

		on_rotation_change();
	}

	auto entity::rotate_absolute(const geometry::orientation_t& rotation) -> void
	{
		m_orientation = rotation;
		m_node_requires_reconstruction = true;

		on_rotation_change();
	}

	auto entity::scale_absolute(const geometry::scale_t& scaling) -> void
	{
		m_scale = scaling;
		m_node_requires_reconstruction = true;

		on_scale_change();
	}

	auto entity::local_transformation(const geometry::transformation_t& transformation) -> void
	{
		m_node->local_transformation(transformation);
	}

	auto entity::local_transformation() const -> const geometry::transformation_t&
	{
		if (m_node_requires_reconstruction)
			reconstruct_node();

		return m_node->local_transformation();
	}

	auto entity::global_transformation() const -> const geometry::transformation_t
	{
		return m_node->global_transformation();
	}

	auto entity::reconstruct_node() const -> void
	{
		auto transformation = geometry::transformation_t {1.0f};

		const auto translate = glm::translate(transformation, m_position);
		const auto rotate = glm::mat4_cast(m_orientation);
		const auto scale = glm::scale(transformation, m_scale);

		m_node->local_transformation(scale * rotate * translate);

		m_node_requires_reconstruction = false;
	}
}
