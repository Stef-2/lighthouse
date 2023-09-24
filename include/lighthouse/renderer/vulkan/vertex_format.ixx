module;

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

export module vertex_format;

export namespace lh
{
	namespace vulkan
	{
		/*
		struct vertex
		{
			glm::vec3 m_position;
			glm::vec3 m_normal;
			glm::vec2 m_tex_coords;
		};*/

		struct vertex
		{
			glm::vec4 m_position;
			glm::vec4 m_normal;
			glm::vec2 m_tex_coords;
		};
	}
}
