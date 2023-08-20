module;
#pragma once

#include "glm/mat4x4.hpp"

export module node;

import std.core;

export namespace lh
{
	export class node
	{
	public:
		// internal transformation type
		using transformation_t = glm::mat4x4;

		// mode that describes what happens to our relatives after we die
		enum class destruction_strategy
		{
			// connect out children with our parent
			collapse,
			// connect our children with the world node
			orphanage
		};

		node(node& parent = s_root_node,
			 const transformation_t& = transformation_t {1.0f},
			 destruction_strategy = destruction_strategy::collapse);
		~node();

		static auto root_node() -> node&;

		auto parent(node&) -> void;
		auto parent() const -> node&;

		auto add_child(node&) -> void;
		auto remove_child(node&) -> void;
		auto children() const -> const std::vector<node*>&;

		auto is_ancestor_of(const node&) const -> bool;
		auto is_descendent_of(const node&) const -> bool;
		auto is_sibling_of(const node&) const -> bool;
		auto descendent_count() const -> const std::size_t;

		auto operator==(const node&) const -> bool;

		auto local_transformation(const transformation_t&) -> void;
		auto local_transformation() const -> const transformation_t&;
		auto global_transformation() const -> const transformation_t&;

	private:
		// remove ourselves from our current parents children list
		// this must be followed with acquisition of a new parent, unless called from the destructor
		auto get_disowned() -> void;

		template <typename F>
		auto traverse_down(const F& function, const node& node) const -> void
		{
			for (auto child : node.children())
			{
				std::invoke(function, *child);

				child->traverse_down(function, *child);
			}
		}

		node* m_parent;
		std::vector<node*> m_children;

		transformation_t m_transformation;

		destruction_strategy m_destruction_mode;

		static node s_root_node;
	};
}
