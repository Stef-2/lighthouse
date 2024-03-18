module;

#if INTELLISENSE
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <algorithm>
#include <cstddef>
#include <concepts>
#endif

export module physical_property;

#if not INTELLISENSE
import glm;
import std.core;
#endif

import math;
import geometry;
import lighthouse_utility;

namespace lh
{
	namespace geometry
	{
		// concepts
		template <typename T>
		concept physical_property = requires { lh::concepts::is_any<T, position_t, direction_t, scale_t>; };

		template <typename T>
		concept constrainable_property = requires(T t) { glm::clamp(t, t, t); } and physical_property<T> or
										 std::is_same_v<T, direction_t>;

		// static physical property type information
		enum class property_class
		{
			position,
			direction,
			scale
		};

		// initial physical property values
		template <property_class T>
		constexpr auto default_value()
		{
			if constexpr (T == property_class::position) return position_t {0.0, 0.0, 0.0};
			if constexpr (T == property_class::direction) return quaternion_t {1.0, 0.0, 0.0, 0.0};
			if constexpr (T == property_class::scale) return scale_t {1.0, 1.0, 1.0};
		}
		
		// base physical property
		template <physical_property T, property_class Y>
		class base_physical_property : protected T
		{
		public:
			using T::T;

			base_physical_property() : T {default_value<Y>()} {}
			base_physical_property(const T& value) : T {value} {};
			base_physical_property& operator=(const T& value) { *this = value; return *this; };

			operator const T&() const { return static_cast<const T&>(*this); };
			operator T&() { return static_cast<T&>(*this); };

		protected:
			auto value() const -> const T& { return *this; };
			auto modify_relative(const T& value)
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, direction_t>)
					*this *= static_cast<quaternion_t>(value);
				else
					*this += value;
			};

			auto modify_absolute(const T& value) { static_cast<T&>(*this) = value; };
		};

		// constrained physical property
		template <constrainable_property T, property_class Y>
		class constrained_physical_property : public base_physical_property<T, Y>
		{
		public:
			using base_physical_property<T, Y>::base_physical_property;

			// direction_t is constrained by normal_t, position_t and scale_t by their respective type
			using constraint_t = std::conditional<std::is_same_v<T, direction_t>, normal_t, T>::type;

			constrained_physical_property(const base_physical_property<T, Y>& value)
				: base_physical_property<T, Y> {value}, m_lower_bound {}, m_upper_bound {} {}
			constrained_physical_property(const T& value,
										  const constraint_t& lower_bound,
										  const constraint_t& upper_bound)
				: base_physical_property<T, Y> {value}, m_lower_bound {lower_bound}, m_upper_bound {upper_bound} {}

			auto lower_bound(const constraint_t& lower_bound) { m_lower_bound = lower_bound; }
			auto lower_bound() const -> const T& { return m_lower_bound; }

			auto upper_bound(const constraint_t& upper_bound) { m_upper_bound = upper_bound; }
			auto upper_bound() const -> const T& { return m_upper_bound; }

		protected:
			template <typename Y>
				requires constrainable_property<Y>
			auto modify_relative(const Y& value) -> void
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, direction_t>)
				{
					auto direction = constraint_t {};

					if constexpr (std::is_same_v<Y, direction_t>)
						direction = glm::eulerAngles(value);
					else
						direction = value;

					const auto euler_angles = glm::eulerAngles(static_cast<direction_t&>(*this));
					static_cast<T&>(*this) = glm::clamp(euler_angles + direction, m_lower_bound, m_upper_bound);
				}
				else
					static_cast<T&>(*this) = glm::clamp(static_cast<T&>(*this) + value, m_lower_bound, m_upper_bound);
			};

			template <typename Y>
				requires constrainable_property<Y>
			auto modify_absolute(const Y& value)
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, direction_t>)
				{
					auto direction = constraint_t {};

					if constexpr (std::is_same_v<Y, direction_t>)
						direction = glm::eulerAngles(value);
					else
						direction = value;
					static_cast<T&>(*this) = glm::clamp(direction, m_lower_bound, m_upper_bound);
				}
				else
				static_cast<T&>(*this) = glm::clamp(value, m_lower_bound, m_upper_bound);
			}

			constraint_t m_lower_bound {};
			constraint_t m_upper_bound {};
		};
		
		// optional virtual callbacks on property modification
		class property_modification_callback
		{
		protected:
			virtual auto position_modified() -> void {}
			virtual auto direction_modified() -> void {}
			virtual auto scale_modified() -> void {}
		};

		// ==========================================================================
		// instantiations
		// ==========================================================================

		// if compiling on msvc, force 4 byte data alignment for the following classes to save memory
		#if _MSC_VER
		#pragma pack(push, 4)
		#endif

		// position
		template <typename T = base_physical_property<position_t, property_class::position>, typename Y = lh::empty>
			requires lh::concepts::is_any<T,
										  base_physical_property<position_t, property_class::position>,
										  constrained_physical_property<position_t, property_class::position>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class position_property : public T, public Y
		{
		public:
			using T::T;

			auto position() const { return T::value(); };
			auto translate_relative(const normal_t& direction, scalar_t magnitude) { T::modify_relative(direction * magnitude); invoke_callback(); }
			auto translate_relative(const position_t& value) { T::modify_relative(value); invoke_callback(); }
			auto translate_relative(scalar_t x, scalar_t y, scalar_t z) { T::modify_relative({x, y, z}); invoke_callback(); }
			auto translate_absolute(const position_t& value) { T::modify_absolute(value); invoke_callback(); }
			auto translate_absolute(scalar_t x, scalar_t y, scalar_t z) { T::modify_absolute({x, y, z}); invoke_callback(); }

		private:
			auto invoke_callback() { if constexpr (std::is_same_v<Y, property_modification_callback>) this->position_modified(); }
		};
		
		// direction
		template <typename T = base_physical_property<direction_t, property_class::direction>, typename Y = lh::empty>
			requires lh::concepts::is_any<T,
										  base_physical_property<direction_t, property_class::direction>,
										  constrained_physical_property<direction_t, property_class::direction>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class direction_property : public T, public Y
		{
		public:
			using T::T;

			auto direction() const { return T::value(); };
			operator normal_t() { return direction_t::euler_radians_cast(); }
			operator const normal_t() const { return direction_t::euler_radians_cast(); };

			auto rotate_relative(const direction_t& value) { T::modify_relative(value); invoke_callback(); }
			auto rotate_relative(scalar_t w, scalar_t x, scalar_t y, scalar_t z) { T::modify_relative({w, x, y, z}); invoke_callback(); }
			auto rotate_absolute(const direction_t& value) { T::modify_absolute(value); invoke_callback(); }
			auto rotate_absolute(scalar_t w, scalar_t x, scalar_t y, scalar_t z) { T::modify_absolute({w, x, y, z}); invoke_callback(); }

			auto rotate_relative(const normal_t& value) { T::modify_relative(direction_t {value}); invoke_callback(); }
			auto rotate_relative(scalar_t x, scalar_t y, scalar_t z) { T::modify_relative(direction_t {x, y, z}); invoke_callback(); }
			auto rotate_absolute(const normal_t& value) { T::modify_absolute(direction_t {value}); invoke_callback(); }
			auto rotate_absolute(scalar_t x, scalar_t y, scalar_t z) { T::modify_absolute(direction_t {x, y, z}); invoke_callback(); }

		private:
			auto invoke_callback() { if constexpr (std::is_same_v<Y, property_modification_callback>) this->direction_modified(); }
		};

		// scale
		template <typename T = base_physical_property<scale_t, property_class::scale>, typename Y = lh::empty>
			requires lh::concepts::is_any<T,
										  base_physical_property<scale_t, property_class::scale>,
										  constrained_physical_property<scale_t, property_class::scale>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class scale_property : public T, public Y
		{
		public:
			using T::T;

			auto scale() const { return T::value(); };
			auto scale_relative(const scale_t& value) { T::modify_relative(value); invoke_callback(); }
			auto scale_relative(scalar_t x, scalar_t y, scalar_t z) { T::modify_relative({x, y, z}); invoke_callback(); }
			auto scale_absolute(const scale_t& value) { T::modify_absolute(value); invoke_callback(); }
			auto scale_absolute(scalar_t x, scalar_t y, scalar_t z) { T::modify_absolute({x, y, z}); invoke_callback(); }

		private:
			auto invoke_callback() { if constexpr (std::is_same_v<Y, property_modification_callback>) this->scale_modified(); }
		};

		#if _MSC_VER
		#pragma pack(pop)
		#endif
	}
}

export namespace lh
{
	namespace geometry
	{
		using position = position_property<base_physical_property<position_t, property_class::position>>;
		using position_with_callback = position_property<base_physical_property<position_t, property_class::position>, property_modification_callback>;
		using constrained_position = position_property<constrained_physical_property<position_t, property_class::position>>;
		using constrained_position_with_callback = position_property<constrained_physical_property<position_t, property_class::position>, property_modification_callback>;

		using direction = direction_property<base_physical_property<direction_t, property_class::direction>>;
		using direction_with_callback = direction_property<base_physical_property<direction_t, property_class::direction>, property_modification_callback>;
		using constrained_direction = direction_property<constrained_physical_property<direction_t, property_class::direction>>;
		using constrained_direction_with_callback = direction_property<constrained_physical_property<direction_t, property_class::direction>, property_modification_callback>;

		using scale = scale_property<base_physical_property<scale_t, property_class::scale>>;
		using scale_with_callback = scale_property<base_physical_property<scale_t, property_class::scale>, property_modification_callback>;
		using constrained_scale = scale_property<constrained_physical_property<scale_t, property_class::scale>>;
		using constrained_scale_with_callback = scale_property<constrained_physical_property<scale_t, property_class::scale>, property_modification_callback>;

		struct transformable : public position, public direction, public scale
		{};

		struct const_trans : public constrained_position, public constrained_direction, public constrained_scale
		{};
		
		#pragma optimize("", off)
		void func()
		{
			glm::vec3 v {1.0, 1.0, 1.0};
			glm::vec3 f {v};

			transformable t;

			t.translate_relative(5.0f, 0.0f, 10.0f);
			t.translate_absolute(0.0f, 0.0f, 10.0f);

			t.rotate_relative(1.0, 30.0_deg, 0.0_deg, 0.0_deg);
			t.rotate_absolute(0.0_deg, 0.0_deg, 60.0_deg);

			t.scale_relative(1.2f, 0.0f, 3.0f);
			t.scale_absolute(1.2f, 0.0f, 3.0f);

			exit(0);
		}
	}
}