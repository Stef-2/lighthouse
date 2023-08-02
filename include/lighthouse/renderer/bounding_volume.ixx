module;
#pragma once

#include "glm/vec3.hpp"

export module bounding_volume;

export namespace lh
{
	struct bounding_volume
	{ /*
		 template <typename T>
			 requires std::derived_from<T, bounding_volume>
		 auto collides_with(const T&) const -> bool;*/
	};

	struct bounding_box : public bounding_volume
	{
		glm::vec3 m_minima;
		glm::vec3 m_maxima;
	};
}
