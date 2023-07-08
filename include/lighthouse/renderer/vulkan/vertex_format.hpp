#pragma once

namespace lh
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
			glm::vec4 m_color;
		};
	}
}
