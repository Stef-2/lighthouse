#pragma once

#include "glm.hpp"

#include <vector>

namespace lh
{
	class node
	{
	public:
		using transformation = glm::mat4x4;

		// mode that describes what happens to our relatives after we die
		enum class destruction_mode
		{
			// connect out children with our parent
			collapse,
			// connect our children with the world node
			orphanage
		};

		node(node& parent = world_node, transformation = {}, destruction_mode = destruction_mode::collapse);
		~node();

		auto set_parent(node&) -> void;
		auto get_parent() -> node&;

		auto add_child(node&) -> void;
		auto remove_child(node&) -> void;
		auto get_children() -> std::vector<node*>&;

		auto is_ancestor_of(const node&) -> bool;
		auto is_descendent_of(const node&) -> bool;
		auto is_sibling_of(const node&) -> bool;

		auto operator==(const node&) -> bool;

		auto get_local_transformation() -> transformation;
		auto get_global_transformation() -> transformation;

	private:
		// remove ourselves from our current parents children list
		// this must be followed with acquisition of a new parent, unless called from the destructor
		auto get_disowned() -> void;

		node* m_parent;
		std::vector<node*> m_children;

		transformation m_transformation;
		
		destruction_mode m_destruction_mode;

		static node world_node;
	};
}