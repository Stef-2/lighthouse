#include "node.hpp"

lh::node lh::node::world_node {world_node};

lh::node::node(node& parent, transformation transformation, destruction_mode destruction_mode)
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
            child->set_parent(*m_parent);
            m_parent->add_child(*child);
        }

        
    }
    else if (m_destruction_mode == destruction_mode::orphanage)
    {
        for (auto& child : m_children)
            child->set_parent(world_node);
    }

    get_disowned();
}

auto lh::node::set_parent(node& new_parent) -> void
{
    get_disowned();

    new_parent.add_child(*this);
    m_parent = &new_parent;
}

auto lh::node::get_parent() -> node&
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

auto lh::node::get_children() -> std::vector<node*>&
{
    return m_children;
}

auto lh::node::is_ancestor_of(const node& node) -> bool
{
    auto result = false;

    std::function<void(lh::node&, lh::node&)> traverse_down = [&traverse_down, &result](lh::node& parent, lh::node& node)
    {
        for (auto& child : parent.get_children())
        {
            if (*child == node)
            {
                result = true;
                return;
            }
        }

        for (auto& child : parent.get_children())
            traverse_down(*child, node);
    };

    return result;
}

auto lh::node::is_descendent_of(const node& node) -> bool
{
    auto parent = m_parent;

    while (*parent != world_node)
    {
        if (*parent == node)
            return true;

        parent = &parent->get_parent();
    }

    return false;
}

auto lh::node::is_sibling_of(const node& node) -> bool
{
    return (std::find(m_parent->get_children().begin(), m_parent->get_children().end(), node) != m_parent->get_children().end());
}

auto lh::node::get_local_transformation() -> transformation
{
    return m_transformation;
}

auto lh::node::get_global_transformation() -> transformation
{
    auto global_transformation = m_transformation;
    auto parent = m_parent;

    while (*parent != world_node)
    {
        global_transformation *= parent->get_local_transformation();
        parent = &parent->get_parent();
    }

    return global_transformation;
}

auto lh::node::operator==(const node& node) -> bool
{
    return this == &node;
}

auto lh::node::get_disowned() -> void
{
    std::erase(m_parent->get_children(), this);
}

