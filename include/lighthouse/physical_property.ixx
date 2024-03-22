module;

#if INTELLISENSE
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <algorithm>
#include <cstddef>
#include <concepts>
#include <functional>
#include <type_traits>
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
		void foo(int i) {}
		// concepts
		template <typename T>
		concept physical_property = requires { lh::concepts::is_any<T, position_t, rotation_t, scale_t>; };

		template <typename T>
		concept constrainable_property = requires(T t) { glm::clamp(t, t, t); } and physical_property<T> or
										 std::is_same_v<T, rotation_t>;

		// static physical property type information
		enum class property_class
		{
			position,
			rotation,
			scale
		};

		template <typename T>
			requires physical_property<T>
		struct default_values
		{
			// rotation_t is constrained by normal_t, position_t and scale_t by their respective type
			using constraint_t = std::conditional<std::is_same_v<T, rotation_t>, normal_t, T>::type;

			constraint_t m_lower_bound;
			T m_default;
			constraint_t m_upper_bound;
		};

		// initial physical property values
		template <property_class T>
		constexpr auto default_value()
		{
			if constexpr (T == property_class::position)
				return default_values<position_t> {.m_lower_bound {-32'000.0, -32'000.0, -32'000.0},
												   .m_default {0.0, 0.0, 0.0},
												   .m_upper_bound{+32'000.0, +32'000.0, +32'000.0}};
			if constexpr (T == property_class::rotation)
				return default_values<rotation_t> {.m_lower_bound {0.0_deg, 0.0_deg, 0.0_deg},
													.m_default {1.0, 0.0, 0.0, 0.0},
													.m_upper_bound{360.0_deg, 360.0_deg, 360.0_deg}};
			if constexpr (T == property_class::scale)
				return default_values<scale_t> {.m_lower_bound {0.0, 0.0, 0.0},
												.m_default {1.0, 1.0, 1.0},
												.m_upper_bound {1.0, 1.0, 1.0}};
		}
		
		// base physical property
		template <physical_property T, property_class Y>
		class base_physical_property : protected T
		{
		public:
			using T::T;

			base_physical_property() : T {default_value<Y>().m_default} {}
			base_physical_property(const T& value) : T {value} {};
			base_physical_property& operator=(const T& value) { *this = value; return *this; };

			operator const T&() const { return static_cast<const T&>(*this); };
			operator T&() { return static_cast<T&>(*this); };

		protected:
			auto value() const -> const T& { return *this; };
			auto modify_relative(const T& value)
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, rotation_t>)
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

			// rotation_t is constrained by normal_t, position_t and scale_t by their respective type
			using constraint_t = std::conditional<std::is_same_v<T, rotation_t>, normal_t, T>::type;

			constrained_physical_property()
				: base_physical_property<T, Y> {},
				  m_lower_bound {default_value<Y>().m_lower_bound},
				  m_upper_bound {default_value<Y>().m_upper_bound} {}

			constrained_physical_property(const base_physical_property<T, Y>& value)
				: base_physical_property<T, Y> {value},
				  m_lower_bound {default_value<Y>().m_lower_bound},
				  m_upper_bound {default_value<Y>().m_upper_bound} {}

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
				if constexpr (std::is_same_v<T, rotation_t>)
				{
					auto rotation = constraint_t {};

					if constexpr (std::is_same_v<Y, rotation_t>)
						rotation = glm::eulerAngles(value);
					else
						rotation = value;

					const auto euler_angles = glm::eulerAngles(static_cast<rotation_t&>(*this));
					*this = glm::clamp(euler_angles + rotation, m_lower_bound, m_upper_bound);
				}
				else
					*this = glm::clamp(static_cast<T&>(*this) + value, m_lower_bound, m_upper_bound);
			};

			template <typename Y>
				requires constrainable_property<Y>
			auto modify_absolute(const Y& value)
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, rotation_t>)
				{
					auto rotation = constraint_t {};

					if constexpr (std::is_same_v<Y, rotation_t>)
						rotation = glm::eulerAngles(value);
					else
						rotation = value;
					*this = glm::clamp(rotation, m_lower_bound, m_upper_bound);
				}
				else
				*this = glm::clamp(value, m_lower_bound, m_upper_bound);
			}

			constraint_t m_lower_bound {};
			constraint_t m_upper_bound {};
		};
		
		// optional virtual callbacks on property modification
		class property_modification_callback
		{
		protected:
			virtual auto position_modified() -> void {}
			virtual auto rotation_modified() -> void {}
			virtual auto scale_modified() -> void {}
		};

		// ==========================================================================
		// instantiations
		// ==========================================================================

		// if compiling on msvc, force 4 byte data alignment for the following classes to save memory
		#if _MSC_VER
		#pragma pack(push, 4)
		#endif

		// ---- position ---- 
		template <typename T = base_physical_property<position_t, property_class::position>, typename Y = lh::empty>
			requires lh::concepts::is_any<T,
										  base_physical_property<position_t, property_class::position>,
										  constrained_physical_property<position_t, property_class::position>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class position_property : public T, public Y
		{
		public:
			using T::T;
			using position = T;

			auto position_value() const { return T::value(); };

			auto translate_relative(const normal_t& rotation, scalar_t magnitude) { T::modify_relative(rotation * magnitude); invoke_callback(); }
			auto translate_relative(const position_t& value) { T::modify_relative(value); invoke_callback(); }
			auto translate_relative(scalar_t x, scalar_t y, scalar_t z) { T::modify_relative(position_t {x, y, z}); invoke_callback(); }
			auto translate_absolute(const position_t& value) { T::modify_absolute(value); invoke_callback(); }
			auto translate_absolute(scalar_t x, scalar_t y, scalar_t z) { T::modify_absolute(position_t {x, y, z}); invoke_callback(); }


		private:
			auto invoke_callback() { if constexpr (std::is_same_v<Y, property_modification_callback>) this->position_modified(); }
		};


		// ---- rotation ----
		template <typename T = base_physical_property<rotation_t, property_class::rotation>, typename Y = lh::empty>
			requires lh::concepts::is_any<T,
										  base_physical_property<rotation_t, property_class::rotation>,
										  constrained_physical_property<rotation_t, property_class::rotation>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class rotation_property : public T, public Y
		{
		public:
			using T::T;
			using rotation = T;

			auto rotation_value() const { return T::value(); };
			operator normal_t() { return rotation_t::euler_radians_cast(); }
			operator const normal_t() const { return rotation_t::euler_radians_cast(); };

			template <typename... Ts>
			auto rotate_relative(Ts&&... ts)
			{
				delegate_relative_rotation_call(std::forward<Ts>(ts)...);

				invoke_callback();
			}

			template <typename... Ts>
			auto rotate_absolute(Ts&&... ts)
			{
				// delegate the call to one of the handlers
				if constexpr (lh::function_parameters::function_traits<void(const rotation_t&)>::match<Ts...>())
					rotate_absolute_rotation(std::forward<Ts>(ts)...);
				else if constexpr (lh::function_parameters::function_traits<void(scalar_t, scalar_t, scalar_t, scalar_t)>::match<Ts...>())
					rotate_absolute_quaternion(std::forward<Ts>(ts)...);
				else if constexpr (lh::function_parameters::function_traits<void(scalar_t, scalar_t, scalar_t)>::match<Ts...>())
					rotate_absolute_euler(std::forward<Ts>(ts)...);
				else
					static_assert(dependent_false<ts...>::value, "could not statically resolve rotation");

				invoke_callback();
			}

		private:
			template <typename... Ts>
			auto delegate_relative_rotation_call(Ts&&... ts)
			{
				if constexpr (lh::function_parameters::function_traits<void(const rotation_t&)>::match<Ts...>())
					rotate_relative_rotation(std::forward<Ts>(ts)...);
				else if constexpr (lh::function_parameters::function_traits<void(scalar_t, scalar_t, scalar_t, scalar_t)>::match<Ts...>())
					rotate_relative_quaternion(std::forward<Ts>(ts)...);
				else if constexpr (lh::function_parameters::function_traits<void(scalar_t, scalar_t, scalar_t)>::match<Ts...>())
					rotate_relative_euler(std::forward<Ts>(ts)...);
				else
					static_assert(dependent_false<ts...>::value, "could not statically resolve rotation");
			}

			auto rotate_relative_rotation(const rotation_t& value) { T::modify_relative(value); }
			auto rotate_absolute_rotation(const rotation_t& value) { T::modify_absolute(value); }
			auto rotate_relative_quaternion(scalar_t w, scalar_t x, scalar_t y, scalar_t z) { T::modify_relative(rotation_t {w, x, y, z}); }
			auto rotate_absolute_quaternion(scalar_t w, scalar_t x, scalar_t y, scalar_t z) { T::modify_absolute(rotation_t {w, x, y, z}); }
			auto rotate_relative_euler(scalar_t x, scalar_t y, scalar_t z) { T::modify_relative(rotation_t {x, y, z}); }
			auto rotate_absolute_euler(scalar_t x, scalar_t y, scalar_t z) { T::modify_absolute(rotation_t {x, y, z}); }

			auto invoke_callback() { if constexpr (std::is_same_v<Y, property_modification_callback>) this->rotation_modified(); }

		};
		
		// ---- scale ---- 
		template <typename T = base_physical_property<scale_t, property_class::scale>, typename Y = lh::empty>
			requires lh::concepts::is_any<T,
										  base_physical_property<scale_t, property_class::scale>,
										  constrained_physical_property<scale_t, property_class::scale>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class scale_property : public T, public Y
		{
		public:
			using T::T;
			using scale = T;

			auto scale_value() const { return T::value(); };
			auto scale_relative(const scale_t& value) { T::modify_relative(value); invoke_callback(); }
			auto scale_relative(scalar_t x, scalar_t y, scalar_t z) { T::modify_relative(scale_t {x, y, z}); invoke_callback(); }
			auto scale_absolute(const scale_t& value) { T::modify_absolute(value); invoke_callback(); }
			auto scale_absolute(scalar_t x, scalar_t y, scalar_t z) { T::modify_absolute(scale_t {x, y, z}); invoke_callback(); }

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

		using rotation = rotation_property<base_physical_property<rotation_t, property_class::rotation>>;
		using rotation_with_callback = rotation_property<base_physical_property<rotation_t, property_class::rotation>, property_modification_callback>;
		using constrained_rotation = rotation_property<constrained_physical_property<rotation_t, property_class::rotation>>;
		using constrained_rotation_with_callback = rotation_property<constrained_physical_property<rotation_t, property_class::rotation>, property_modification_callback>;

		using scale = scale_property<base_physical_property<scale_t, property_class::scale>>;
		using scale_with_callback = scale_property<base_physical_property<scale_t, property_class::scale>, property_modification_callback>;
		using constrained_scale = scale_property<constrained_physical_property<scale_t, property_class::scale>>;
		using constrained_scale_with_callback = scale_property<constrained_physical_property<scale_t, property_class::scale>, property_modification_callback>;

		
		struct rotatable : public rotation {};

		#pragma optimize("", off)
		void func()
		{
			rotatable r;
			
			//r.rotate_relative(true);
			r.rotate_relative(1.0f, 1.0f, 1.0f);
			r.rotate_absolute(1.0f, 0.0f, 0.0f, 0.0f);



			//exit(0);
		}

		
	}
}