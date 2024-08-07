module;

#if INTELLISENSE
#include "glm/mat4x4.hpp"

#include <vector>
#endif

export module node;

import geometry;

#if not INTELLISENSE
import glm;
#endif

import std;

export namespace lh
{
	export class node
	{
	public:
		// mode that describes what happens to our relatives after we die
		enum class destruction_strategy
		{
			// connect our children with our parent
			collapse,
			// connect our children with the world node
			orphanage
		};

		node(node& parent = s_root_node,
			 const geometry::transformation_t& = geometry::transformation_t {1.0f},
			 destruction_strategy = destruction_strategy::collapse);
		node(const node&) = delete;
		node& operator=(const node&) = delete;
		node(node&&) noexcept;
		node& operator=(node&&) noexcept;
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
		auto operator==(node&) -> bool;

		auto local_transformation(const geometry::transformation_t&) -> void;
		auto local_transformation() const -> const geometry::transformation_t&;
		auto global_transformation() const -> const geometry::transformation_t;

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

		geometry::transformation_t m_transformation;

		destruction_strategy m_destruction_mode;

		static node s_root_node;
	};
}
