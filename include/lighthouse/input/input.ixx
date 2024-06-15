module;

#include "vkfw/vkfw.hpp"

export module input;

import data_type;
import file_system;
import file_type;
import lighthouse_string;
import window;

import std;

namespace lh
{
	// class encapsulating functions with unique IDs
	export class action
	{
	public:
		// internal function type
		using function_t = std::function<void()>;
		// internal guid counter type
		using counter_t = uint64_t;

		// make constructible from invocable / callable concepts
		action(const std::invocable auto& function) : m_action(function), m_guid(s_counter) { s_counter++; }

		auto id() const -> const counter_t&;

		// overlaoded () operator to call the wrapped function
		auto operator()() const -> void;
		// comparison operators for mapping
		auto operator==(const action&) const -> bool;
		auto operator<=>(const action&) const = default;

	private:
		// global guid counter
		static counter_t s_counter;

		function_t m_action;
		counter_t m_guid;
	};

	// utility input namespace, handling key_binding, mouse and file reading
	export namespace input
	{
		enum class input_mode
		{
			game,
			user_interface
		};

		export auto input_mode() -> const decltype(lh::input::input_mode::game)&;
		export auto input_mode(const decltype(lh::input::input_mode::game)&) -> void;
		decltype(lh::input::input_mode::game) s_input_mode;

		class key_binding
		{
		public:
			// full key input, constructed from:
			// (1) keyboard or mouse key code -> defaults to unknown
			// (2) keyboard modifier keys (ctrl, shift, alt, etc) -> defaults to no mods
			// (3) key action (press, release, hold) -> defaults to press
			struct key_input
			{
				key_input(std::variant<vkfw::Key, vkfw::MouseButton> = {},
						  vkfw::ModifierKeyFlags = {},
						  std::variant<vkfw::KeyAction, vkfw::MouseButtonAction> = vkfw::KeyAction::Press);

				// define comparison and hash functions so it can be used as a map key
				auto operator<=>(const key_input&) const = default;
				auto operator()(const key_input& value) const -> std::size_t;

				// allow either keyboard or mouse key
				std::int32_t m_key;
				std::int32_t m_modifier_flags;
				std::int32_t m_action;
			};

			static auto execute_pressed_keys() -> void;

			static auto key_bindings() -> std::unordered_multimap<const key_input, const action, const key_input>&;
			static auto is_bound(const key_input&) -> const bool;
			static auto bound_keys() -> const std::vector<key_input>;

			// binds action(s) to a key
			static auto bind(const key_input&, const action&) -> void;
			static auto bind(const key_input&, const std::vector<action>&) -> void;

			// unbinds all actions from a key
			static auto unbind(const key_input&) -> void;

			// unbinds a specific action from a key
			static auto unbind(const key_input&, const action&) -> void;
			static auto initialize(const window&) -> void;

		private:

			static window* s_window;
			// map key inputs to any number of actions
			static inline auto s_key_bindings =
				std::unordered_multimap<const key_input, const action, const key_input> {};
			static inline auto s_pressed_keys = std::vector<key_input> {};
		};

		class mouse
		{
		public:
			using parameter_precision_t = double;

			struct move_info
			{
				struct screen_position
				{
					parameter_precision_t x;
					parameter_precision_t y;
				};

				screen_position m_current;
				screen_position m_previous;
			};

			using on_move_action_t = std::function<void(const move_info&)>;

			static auto initialize(const window&) -> void;
			static auto move_callback(const on_move_action_t&) -> void;

		private:
			static window* s_window;
			static parameter_precision_t s_previous_x;
			static parameter_precision_t s_previous_y;
		};

		

		export auto initialize(const window&) -> void;
	};
}