#include "node.hpp"

#include <functional>

lh::node lh::node::world_node {world_node};

auto lh::node::get_world_node() -> lh::node&
{
	return world_node;
}

lh::node::node(node& parent, transformation_t transformation, destruction_mode destruction_mode)
	: m_parent(&parent), m_transformation(transformation), m_destruction_mode(destruction_mode), m_children {}
{
	m_parent->add_child(*this);
}

lh::node::~node()
{
	if (m_destruction_mode == destruction_mode::collapse)
	{
		for (auto& child : m_children)
		{
			child->parent(*m_parent);
			m_parent->add_child(*child);
		}
	} else if (m_destruction_mode == destruction_mode::orphanage)
	{
		for (auto& child : m_children)
			child->parent(world_node);
	}

	get_disowned();
}

auto lh::node::parent(node& new_parent) -> void
{
	get_disowned();

	new_parent.add_child(*this);
	m_parent = &new_parent;
}

auto lh::node::parent() const -> node&
{
	return *m_parent;
}

auto lh::node::add_child(node& child) -> void
{
	m_children.push_back(&child);
}

auto lh::node::remove_child(node& child) -> void
{
	std::erase(m_children, &child);
}

auto lh::node::children() -> std::vector<node*>&
{
	return m_children;
}

auto lh::node::is_ancestor_of(const node& node) -> bool
{
	auto result = false;

	std::function<void(lh::node&, const lh::node&)> traverse_down = [&traverse_down, &result](lh::node& parent,
																							  const lh::node& node) {
		for (const auto& child : parent.children())
		{
			if (*child == node)
			{
				result = true;
				return;
			}
		}

		for (auto& child : parent.children())
			traverse_down(*child, node);
	};

	traverse_down(*this, node);

	return result;
}

auto lh::node::is_descendent_of(const node& node) const -> bool
{
	auto parent = m_parent;

	while (*parent == world_node)
	{
		if (*parent == node)
			return true;

		parent = &parent->parent();
	}

	return false;
}

auto lh::node::is_sibling_of(const node& node) const -> bool
{
	return (std::find(m_parent->children().begin(), m_parent->children().end(), &node) != m_parent->children().end());
}

auto lh::node::local_transformation(const transformation_t& transformation) -> void
{
	m_transformation = transformation;
}

auto lh::node::local_transformation() const -> transformation_t
{
	return m_transformation;
}

auto lh::node::global_transformation() const -> transformation_t
{
	auto global_transformation = m_transformation;
	auto parent = m_parent;

	while (*parent != world_node)
	{
		global_transformation *= parent->local_transformation();
		parent = &parent->parent();
	}

	return global_transformation;
}

auto lh::node::operator==(const node& node) const -> bool
{
	return this == &node;
}

auto lh::node::get_disowned() const -> void
{
	std::erase(m_parent->children(), this);
}
