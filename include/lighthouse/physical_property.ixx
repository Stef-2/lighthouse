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
		struct direction_t;
		using quaternion_t = glm::quat;
		// ==========================================================================

		// concepts
		template <typename T>
		concept physical_property = requires { lh::concepts::is_any<T, position_t, direction_t, scale_t>; };

		template <typename T>
		concept constrainable = requires (T t) { glm::clamp(t, t, t); } and physical_property<T> or std::is_same_v<T, direction_t>;

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
				volatile int i = 2323;
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, direction_t>)
				{
					const auto w = static_cast<quaternion_t>(*this) * static_cast<quaternion_t>(value);
					const auto& me = static_cast<direction_t&>(*this);
					*this *= w;
					//(*this) = w;
					std::cout << this->w << " " << i << '\n';
				}
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

			// direction_t is constrained by normal_t, position_t and scale_t by their respective type
			using constrain_t = std::conditional<std::is_same_v<T, direction_t>, normal_t, T>::type;

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
				requires constrainable<Y>
			auto modify_relative(const Y& value) -> void
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, direction_t>)
				{
					auto direction = constrain_t {};

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
				requires constrainable<Y>
			auto modify_absolute(const Y& value)
			{
				// special modification rules for quaternions
				if constexpr (std::is_same_v<T, direction_t>)
				{
					auto direction = constrain_t {};

					if constexpr (std::is_same_v<Y, direction_t>)
						direction = glm::eulerAngles(value);
					else
						direction = value;
					static_cast<T&>(*this) = glm::clamp(direction, m_lower_bound, m_upper_bound);
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
		
		// direction
		template <typename T = base_physical_property<direction_t>, typename Y = lh::empty>
			requires lh::concepts::is_any<T, base_physical_property<direction_t>, constrained_physical_property<direction_t>> and
					 lh::concepts::is_any<Y, lh::empty, property_modification_callback>
		class direction_property : public T, public Y
		{
		public:
			using T::T;

			auto direction() const { return T::value(); };
			operator const normal_t() const { return glm::eulerAngles(*this); };

			auto rotate_relative(const direction_t& value) { T::modify_relative(value); invoke_callback(); }
			auto rotate_absolute(const direction_t& value) { T::modify_absolute(value); invoke_callback(); }

			auto rotate_relative(const normal_t& value) { T::modify_relative(direction_t {value}); invoke_callback(); }
			auto rotate_absolute(const normal_t& value) { T::modify_absolute(direction_t {value}); invoke_callback(); }

		private:
			auto invoke_callback() { if constexpr (std::is_same_v<Y, property_modification_callback>) this->direction_modified(); }
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
		struct direction_t : public quaternion_t
		{
			using quaternion_t::quaternion_t;

			direction_t();
			direction_t(const quaternion_t&);
			direction_t(const normal_t&);
			auto operator=(const quaternion_t& value) -> void { static_cast<quaternion_t>(*this) = value; }
			auto operator=(const normal_t& value) -> void { static_cast<quaternion_t>(*this) = quaternion_t {value}; }

			auto rotate(const quaternion_t&) -> void;
			auto rotate(const normal_t&) -> void;

			auto euler_degrees_cast() const -> const normal_t;
			auto euler_radians_cast() const -> const normal_t;
			auto matrix_cast() const -> const transformation_t;

			auto dot_product(const direction_t& other) const -> const scalar_t;
			auto cross_product(const direction_t& other) const -> const quaternion_t;

			operator quaternion_t&();
			operator const quaternion_t&() const;
			operator normal_t();
			operator const normal_t();
			operator transformation_t();
			operator const transformation_t() const;
		};

		using position = position_property<base_physical_property<position_t>>;
		using position_with_callback = position_property<base_physical_property<position_t>, property_modification_callback>;
		using constrained_position = position_property<constrained_physical_property<position_t>>;
		using constrained_position_with_callback = position_property<constrained_physical_property<position_t>, property_modification_callback>;

		using direction = direction_property<base_physical_property<direction_t>>;
		using direction_with_callback = direction_property<base_physical_property<direction_t>, property_modification_callback>;
		using constrained_direction = direction_property<constrained_physical_property<direction_t>>;
		using constrained_direction_with_callback = direction_property<constrained_physical_property<direction_t>, property_modification_callback>;

		using scale = scale_property<base_physical_property<scale_t>>;
		using scale_with_callback = scale_property<base_physical_property<scale_t>, property_modification_callback>;
		using constrained_scale = scale_property<constrained_physical_property<scale_t>>;
		using constrained_scale_with_callback = scale_property<constrained_physical_property<scale_t>, property_modification_callback>;

		struct transformable : public position, public direction, public scale
		{};

		struct const_trans : public constrained_position, public constrained_direction, public constrained_scale
		{};
		
		#pragma optimize("", off)
		void func()
		{
			auto ang = glm::radians(normal_t {60.0f, 0.0f, 0.0f});

			glm::quat q {1.0f, 1.0f, 1.0f, 1.0f};
			q = glm::normalize(q);

			glm::quat p {1.0f, 0.0f, 0.0f, 0.0f};

			glm::quat x {glm::radians(normal_t{220.0f, 0.0f, 00.0f})};
			glm::quat y {glm::radians(normal_t{0.0f, 90.0f, 0.0f})};
			glm::quat z {glm::radians(normal_t{0.0f, 0.0f, 90.0f})};

			direction_t d = p;
			direction_t b = x;
			const auto test = (quaternion_t)d * (quaternion_t)b;

			p = p * x;
			p = p * y;
			//p = p * z;

			transformable t;

			t.translate_relative({5.0f, 0.0f, 10.0f});
			t.translate_absolute({0.0f, 0.0f, 10.0f});

			t.rotate_relative(p);

			t.rotate_absolute(glm::radians(normal_t{0.0f, 0.0f, 60.0f}));

			exit(0);
		}
	}
}