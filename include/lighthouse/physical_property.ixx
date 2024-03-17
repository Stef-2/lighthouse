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

import geometry;
import lighthouse_utility;

namespace lh
{
	namespace geometry
	{
		// concepts
		template <typename T>
		concept physical_property = requires { lh::concepts::is_any<T, position_t, orientation_t, scale_t>; };

		template <typename T>
		concept constrainable = requires (T t) { glm::clamp(t, t, t); } and physical_property<T>;

		// base
		template <physical_property T>
		class base_physical_property : protected T
		{
		public:
			using T::T;

			base_physical_property() : T {} {}
			base_physical_property(const T& value) : T {value} {};
			base_physical_property& operator=(const T& value) { *this = value; return *this; };

			operator const T&() const { return static_cast<const T&>(*this); };
			operator T&() { return static_cast<T&>(*this); };

		protected:
			auto value() const -> const T& { return *this; };
			auto modify_relative(const T& value)
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, orientation_t>)
					*this *= value;
				else
					*this += value;
			};
			auto modify_absolute(const T& value) { static_cast<T&>(*this) = value; };
		};

		// constrained base
		template <constrainable T>
		class constrained_physical_property : public base_physical_property<T>
		{
		public:
			using base_physical_property<T>::base_physical_property;

			// orientation_t is constrained by normal_t
			using constrain_t = std::conditional<std::is_same_v<T, orientation_t>, normal_t, T>::type;

			constrained_physical_property(const base_physical_property<T>& value)
				: base_physical_property<T> {value}, m_lower_bound {}, m_upper_bound {} {}
			constrained_physical_property(const T& value,
										  const constrain_t& lower_bound,
										  const constrain_t& upper_bound)
				: base_physical_property<T> {value}, m_lower_bound {lower_bound}, m_upper_bound {upper_bound} {}

			auto lower_bound(const constrain_t& lower_bound) { m_lower_bound = lower_bound; }
			auto lower_bound() const -> const T& { return m_lower_bound; }

			auto upper_bound(const constrain_t& upper_bound) { m_upper_bound = upper_bound; }
			auto upper_bound() const -> const T& { return m_upper_bound; }

		protected:
			template <typename Y>
				requires constrainable<Y> or std::is_same_v<Y, normal_t>
			auto modify_relative(const Y& value) -> void
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, orientation_t>)
				{
					auto direction = constrain_t {};

					if constexpr (std::is_same_v<Y, orientation_t>)
						direction = glm::eulerAngles(value);
					else
						direction = value;

					const auto euler_angles = glm::eulerAngles(static_cast<orientation_t&>(*this));
					static_cast<T&>(*this) = glm::clamp(euler_angles + direction, m_lower_bound, m_upper_bound);
				}
				else
					static_cast<T&>(*this) = glm::clamp(static_cast<T&>(*this) + value, m_lower_bound, m_upper_bound);
			};

			template <typename Y>
				requires constrainable<Y> or std::is_same_v<Y, normal_t>
			auto modify_absolute(const Y& value)
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, orientation_t>)
				{
					auto direction = constrain_t {};

					if constexpr (std::is_same_v<Y, orientation_t>)
						direction = glm::eulerAngles(value);
					else
						direction = value;
					static_cast<T&>(*this) = glm::clamp(glm::eulerAngles(value), m_lower_bound, m_upper_bound);
				}
				else
				static_cast<T&>(*this) = glm::clamp(value, m_lower_bound, m_upper_bound);
			}

			constrain_t m_lower_bound {};
			constrain_t m_upper_bound {};
		};
		
		// optional virtual callbacks on property modification
		class property_modification_callback
		{
		protected:
			virtual auto position_modified() -> void {}
			virtual auto orientation_modified() -> void {}
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
		template <typename T = base_physical_property<position_t>, typename Y = lh::empty>
			requires lh::concepts::is_any<T, base_physical_property<position_t>, constrained_physical_property<position_t>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class position_property : public T, public Y
		{
		public:
			using T::T;

			auto position() const { return T::value(); };
			auto translate_relative(const normal_t& direction, scalar_t magnitude) { T::modify_relative(direction * magnitude); invoke_callback(); }
			auto translate_relative(const position_t& value) { T::modify_relative(value); invoke_callback(); }
			auto translate_absolute(const position_t& value) { T::modify_absolute(value); invoke_callback(); }

		private:
			auto invoke_callback() { if constexpr (std::is_same_v<Y, property_modification_callback>) this->position_modified(); }
		};
		
		// orientation
		template <typename T = base_physical_property<orientation_t>, typename Y = lh::empty>
			requires lh::concepts::is_any<T, base_physical_property<orientation_t>, constrained_physical_property<orientation_t>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class orientation_property : public T, public Y
		{
		public:
			using T::T;

			auto orientation() const { return T::value(); };
			auto direction() const -> const normal_t { return glm::eulerAngles(*this); };
			operator const normal_t() const { return glm::eulerAngles(*this); };

			auto rotate_relative(const orientation_t& value) { T::modify_relative(value); invoke_callback(); }
			auto rotate_absolute(const orientation_t& value) { T::modify_absolute(value); invoke_callback(); }

			auto rotate_relative(const normal_t& value) { T::modify_relative(orientation_t {value}); invoke_callback(); }
			auto rotate_absolute(const normal_t& value) { T::modify_absolute(orientation_t {value}); invoke_callback(); }

		private:
			auto invoke_callback() { if constexpr (std::is_same_v<Y, property_modification_callback>) this->orientation_modified(); }
		};

		// scale
		template <typename T = base_physical_property<scale_t>, typename Y = lh::empty>
			requires lh::concepts::is_any<T, base_physical_property<scale_t>, constrained_physical_property<scale_t>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class scale_property : public T, public Y
		{
		public:
			using T::T;

			auto scale() const { return T::value(); };
			auto scale_relative(const scale_t& value) { T::modify_relative(value); invoke_callback(); }
			auto scale_absolute(const scale_t& value) { T::modify_absolute(value); invoke_callback(); }

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
		using position = position_property<base_physical_property<position_t>>;
		using position_with_callback = position_property<base_physical_property<position_t>, property_modification_callback>;
		using constrained_position = position_property<constrained_physical_property<position_t>>;
		using constrained_position_with_callback = position_property<constrained_physical_property<position_t>, property_modification_callback>;

		using orientation = orientation_property<base_physical_property<orientation_t>>;
		using orientation_with_callback = orientation_property<base_physical_property<orientation_t>, property_modification_callback>;
		using constrained_orientation = orientation_property<constrained_physical_property<orientation_t>>;
		using constrained_orientation_with_callback = orientation_property<constrained_physical_property<orientation_t>, property_modification_callback>;

		using scale = scale_property<base_physical_property<scale_t>>;
		using scale_with_callback = scale_property<base_physical_property<scale_t>, property_modification_callback>;
		using constrained_scale = scale_property<constrained_physical_property<scale_t>>;
		using constrained_scale_with_callback = scale_property<constrained_physical_property<scale_t>, property_modification_callback>;

		struct transformable : public position, public orientation, public scale
		{};

		struct const_trans : public constrained_position, public constrained_orientation, public constrained_scale
		{};
		
		consteval auto wtf()
		{
			glm::quat q;
			q.w = 1.0f;
			q.x = 0.0f;
			q.y = 0.0f;
			q.z = 1.0f;

			return q;
		}
		
		#pragma optimize("", off)
		void func()
		{
			transformable t;
			const_trans c_t;
			c_t.constrained_position::lower_bound({0.3f, 0.4f, 0.2f});
			c_t.constrained_position::upper_bound({0.3f, 0.4f, 0.2f});
			c_t.constrained_scale::upper_bound({0.8f, 0.9f, 0.8f});
			c_t.scale_relative({3.0f, 2.2f, 0.5f});
			c_t.rotate_relative({1.0f, 0.02f, 5.6f});
			c_t.rotate_relative(orientation_t {normal_t {5.0f, 2.02f, 5.6f}});
			c_t.translate_absolute({2.0f, 4.02f, 33.6f});

			std::cout << t.scale().x;

			direction_t d;
			constexpr auto omg = wtf();

			glm::quat q = glm::quat {glm::radians(glm::vec3(0.0f, 0.0f, 90.0f))};
			q = glm::rotate(q, 90.0f, glm::degrees(glm::vec3(0.0f, 0.0f, 1.0f)));
			//q = glm::clamp(q, glm::quat {0.0f, 0.0f, 0.0f, 0.0f}, glm::quat {1.0f, 1.0f, 1.0f, 1.0f});
			q = glm::vec3 {1.0f, 0.0f, 0.0f};

			std::cout << q.w;
			exit(0);
		}
	}
}