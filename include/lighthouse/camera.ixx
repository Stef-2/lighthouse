module;
#pragma once

export module camera;

#if INTELLISENSE
#include "lighthouse/entity.ixx"
#else
import entity;
#endif

export namespace lh
{
	class camera : public entity
	{
	public:

		struct create_info
		{
			float m_aspect_ratio;
			float m_near_clip;
			float m_far_clip;
			float m_field_of_view;
		};

		camera(const node& = {}, const create_info& = {});

		auto view() const -> const entity::transformation_t;

	private:

		float m_aspect_ratio;
		float m_near_clip;
		float m_far_clip;
		float m_field_of_view;
	};
}