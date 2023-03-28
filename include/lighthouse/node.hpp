#pragma once

#include <vector>

namespace lh
{
	class node
	{
	public:
		// internal transformation type
		using transformation_t = glm::mat4x4;

		// mode that describes what happens to our relatives after we die
		enum class destruction_mode
		{
			// connect out children with our parent
			collapse,
			// connect our children with the world node
			orphanage
		};

		node(node& parent = world_node,
			 transformation_t = transformation_t {1.0f},
			 destruction_mode = destruction_mode::collapse);
		~node();

		static auto get_world_node() -> node&;

		auto parent(node&) -> void;
		auto parent() const -> node&;

		auto add_child(node&) -> void;
		auto remove_child(node&) -> void;
		auto children() -> std::vector<node*>&;

		auto is_ancestor_of(const node&) -> bool;
		auto is_descendent_of(const node&) const -> bool;
		auto is_sibling_of(const node&) const -> bool;

		auto operator==(const node&) const -> bool;

		auto local_transformation(const transformation_t&) -> void;
		auto local_transformation() const -> transformation_t;
		auto global_transformation() const -> transformation_t;

	private:
		// remove ourselves from our current parents children list
		// this must be followed with acquisition of a new parent, unless called from the destructor
		auto get_disowned() const -> void;

		node* m_parent;
		std::vector<node*> m_children;

		transformation_t m_transformation;

		destruction_mode m_destruction_mode;

		static node world_node;
	};
}
