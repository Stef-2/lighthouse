module;

#if INTELLISENSE
#include <algorithm>
#endif

export module physical_property;

#if not INTELLISENSE
import glm;
import std.core;
#endif

import geometry;

export namespace lh
{
	namespace geometry
	{
		template <typename T>
		//requires (T t) {glm::clamp(t, t, t);}
		class physical_property
		{
		public:
			auto property() const -> const T& { return m_property; };

		protected:
			virtual auto modify_relative(const T&) -> void = 0;
			auto modify_absolute(const T& property) -> void { m_property = property; };

			T m_property;
		};

		template <typename T>
		class constrained_physical_property : public physical_property<T>
		{
		public:
			using physical_property<T>::physical_property;

			auto lower_bound(const T& lower_bound) -> void { m_lower_bound = lower_bound; }
			auto lower_bound() const -> const T& { return m_lower_bound; }
			auto upper_bound(const T& upper_bound) -> void { m_upper_bound = upper_bound; }
			auto upper_bound() const -> const T& { return m_upper_bound; }

		protected:
			auto clamp() -> T { return std::clamp(); }
			T m_lower_bound;
			T m_upper_bound;
		};

		class position : physical_property<position_t>
		{
		public:
			auto translate_relative(const position_t&) -> void;
			auto translate_absolute(const position_t&) -> void;
		private:
		};
	}
}