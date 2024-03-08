module;

#if INTELLISENSE
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

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
		// concepts
		template <typename T>
		concept physical_property = requires {
			std::is_same_v<T, position_t> or std::is_same_v<T, orientation_t> or std::is_same_v<T, scale_t>; };

		template <typename T>
		concept constrainable = requires (T t) { glm::clamp(t, t, t); } and physical_property<T>;

		// base
		template <physical_property T>
		class base_physical_property : protected T
		{
		public:
			using T::T;
			base_physical_property(const T& value) : T {value} {};

			auto value() const -> const T& { return *this; };
			operator const T&() const { return static_cast<const T&>(*this); };
			operator T&() { return static_cast<T&>(*this); };

		protected:
			auto modify_relative(const T& value) -> void
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, orientation_t>)
					*this *= value;
				else
					*this += value;
			};
			auto modify_absolute(const T& value) -> void { static_cast<T&>(*this) = value; };
		};

		// constrained
		template <constrainable T>
		class constrained_physical_property : public base_physical_property<T>
		{
		public:
			using base_physical_property<T>::base_physical_property;
			constrained_physical_property(const T& value, const T& lower_bound, const T& upper_bound)
				: base_physical_property<T> {value},
				m_lower_bound {lower_bound},
				m_upper_bound {upper_bound} {}

			auto lower_bound(const T& lower_bound) -> void { m_lower_bound = lower_bound; }
			auto lower_bound() const -> const T& { return m_lower_bound; }

			auto upper_bound(const T& upper_bound) -> void { m_upper_bound = upper_bound; }
			auto upper_bound() const -> const T& { return m_upper_bound; }

		protected:
			auto modify_relative(const T& value) -> void
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, orientation_t>)
					*this = glm::clamp(*this * value, m_lower_bound, m_upper_bound);
				else
					*this = glm::clamp(*this + value, m_lower_bound, m_upper_bound);

			};
			auto modify_absolute(const T& value) -> void {*this = glm::clamp(value, m_lower_bound, m_upper_bound);}

			auto constrain() -> void { *this = std::clamp(static_cast<T&>(*this), m_lower_bound, m_upper_bound); }

			T m_lower_bound {};
			T m_upper_bound {};
		};

		// instantiations
		
		template <typename T = base_physical_property<position_t>>
			requires std::is_same_v<T, base_physical_property<position_t>> or
					 std::is_same_v<T, constrained_physical_property<position_t>>
		class position : public T
		{
		public:
			using T::T;

			auto translate_relative(const position_t&) -> void {};
			auto translate_absolute(const position_t&) -> void;
		};

		template <typename T = base_physical_property<position_t>>
			requires std::is_same_v<T, base_physical_property<position_t>> or
					 std::is_same_v<T, constrained_physical_property<position_t>>
		class orientation : public T
		{
		public:
			using T::T;

			auto value() const -> const rotation_t;
			operator const rotation_t() const;

			auto rotate_relative(const orientation_t&) -> void;
			auto rotate_absolute(const orientation_t&) -> void;

			auto rotate_relative(const rotation_t&) -> void;
			auto rotate_absolute(const rotation_t&) -> void;
		};

		template <typename T = base_physical_property<position_t>>
			requires std::is_same_v<T, base_physical_property<position_t>> or
					 std::is_same_v<T, constrained_physical_property<position_t>>
		class scale : public T
		{
		public:
			using T::T;

			auto scale_relative(const scale_t&) -> void;
			auto scale_absolute(const scale_t&) -> void;
		};

		using wtf_t = position<base_physical_property<position_t>>;
		using omg_t = position<constrained_physical_property<position_t>>;
	}
}

//lh::geometry::position p;
lh::geometry::omg_t t {};
//lh::geometry::wtf_t p {0.0f, 0.0f, 0.0f};
//lh::geometry::position a {0.0f, 0.0f, 0.0f};
//lh::geometry::position b {glm::vec3 {0.0f}};
//lh::geometry::position<lh::geometry::base_physical_property<lh::geometry::position_t>> b {glm::vec3(0.0f, 0.0f, 0.0f)};

void f()
{
	//p.translate_relative(glm::vec3 {0.0f, 0.0f, 0.0f});
}