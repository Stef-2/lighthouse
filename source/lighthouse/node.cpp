module;

#if INTELLISENSE
#include "lighthouse/node.ixx"
#else
module node;
#endif

namespace lh
{

	node node::s_root_node {s_root_node};

	auto node::root_node() -> node&
	{
		return s_root_node;
	}

	node::node(node& parent, transformation_t transformation, destruction_mode destruction_mode)
		: m_parent(&parent), m_transformation(transformation), m_destruction_mode(destruction_mode), m_children {}
	{
		m_parent->add_child(*this);
	}

	node::~node()
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
				child->parent(s_root_node);
		}

		get_disowned();
	}

	auto node::parent(node& new_parent) -> void
	{
		get_disowned();

		new_parent.add_child(*this);
		m_parent = &new_parent;
	}

	auto node::parent() const -> node&
	{
		return *m_parent;
	}

	auto node::add_child(node& child) -> void
	{
		m_children.push_back(&child);
		child.parent() = *this;
	}

	auto node::remove_child(node& child) -> void
	{
		std::erase(m_children, &child);
	}

	auto node::children() const -> const std::vector<node*>&
	{
		return m_children;
	}

	auto node::is_ancestor_of(const node& node) const -> bool
	{
		return node.is_descendent_of(*this);
	}

	auto node::is_descendent_of(const node& node) const -> bool
	{
		auto parent = m_parent;

		while (*parent != s_root_node)
		{
			if (*parent == node)
				return true;

			parent = &parent->parent();
		}

		return false;
	}

	auto node::is_sibling_of(const node& node) const -> bool
	{
		return std::ranges::contains(m_parent->children(), &node);
	}

	auto node::descendent_count() const -> const std::size_t
	{
		auto descendent_count = std::size_t {};

		traverse_down([&descendent_count](const auto&...) { descendent_count++; }, *this);

		return descendent_count;
	}

	auto node::local_transformation(const transformation_t& transformation) -> void
	{
		m_transformation = transformation;
	}

	auto node::local_transformation() const -> const transformation_t&
	{
		return m_transformation;
	}

	auto node::global_transformation() const -> const transformation_t&
	{
		auto global_transformation = m_transformation;
		auto parent = m_parent;

		while (*parent != s_root_node)
		{
			global_transformation *= parent->local_transformation();
			parent = &parent->parent();
		}

		return global_transformation;
	}

	auto node::operator==(const node& node) const -> bool
	{
		return this == &node;
	}

	auto node::get_disowned() -> void
	{
		std::erase(const_cast<std::vector<node*>&>(m_parent->children()), this);
	}
}
