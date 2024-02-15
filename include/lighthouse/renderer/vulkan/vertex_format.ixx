module;

#if INTELLISENSE
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#endif

export module vertex_format;

#if not INTELLISENSE
import glm;
#endif

export namespace lh
{
	namespace vulkan
	{
		struct vertex
		{
			glm::vec3 m_position;
			glm::vec3 m_normal;
			glm::vec3 m_tangent;
			glm::vec3 m_bitangent;
			glm::vec2 m_tex_coords;
		};
	}
}