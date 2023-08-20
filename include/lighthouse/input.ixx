module;

#include "vkfw/vkfw.hpp"

export module input;

import file_system;
import file_type;
import lighthouse_string;
import window;
import std.filesystem;
import std.core;

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

		auto get_id() const -> const counter_t&;

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

	// utiltiy input namespace, handling keyboard, mouse and file reading
	export namespace input
	{
		class keyboard
		{
		public:
			// full key input, constructed from:
			// (1) keyboard code -> defaults to unknown
			// (2) keyboard modifier keys (ctrl, shift, alt, etc) -> defaults to no mods
			// (3) key action (press, release, hold) -> defaults to press
			struct key_input
			{
				key_input(vkfw::Key = {},
						  vkfw::ModifierKeyFlags = {},
						  vkfw::KeyAction = vkfw::KeyAction::Press);

				// define comparison and hash functions so it can be used as a map key
				auto operator<=>(const key_input&) const = default;
				auto operator()(const key_input& value) const -> std::size_t;

				vkfw::Key m_key;
				vkfw::ModifierKeyFlags::MaskType m_flags;
				vkfw::KeyAction m_action;
			};

			static auto key_bindings() -> std::unordered_multimap<const key_input, const action, const key_input>&;

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
		};

		class mouse
		{
		public:
			// full mouse key input, constructed from:
			// (1) mouse key code -> defaults to left mouse button
			// (2) keyboard modifier keys (ctrl, shift, alt, etc) -> defaults to no mods
			// (3) mouse key action (press, release, hold) -> defaults to press
			struct mouse_input
			{
				mouse_input(vkfw::MouseButton = {vkfw::MouseButton::Left},
							vkfw::ModifierKeyFlags = {},
							vkfw::MouseButtonAction = vkfw::MouseButtonAction::Press);

				// define comparison and hash functions so it can be used as a map key
				auto operator<=>(const mouse_input&) const = default;
				auto operator()(const mouse_input& value) const -> std::size_t;

				vkfw::MouseButton m_key;
				vkfw::ModifierKeyFlags::MaskType m_flags;
				vkfw::MouseButtonAction m_action;
			};

			struct move_data
			{
				struct screen_position
				{
					double x;
					double y;
				};

				screen_position m_current;
				screen_position m_previous;
			};

			static auto initialize(const window&) -> void;

			using on_move_action_t = std::function<void(const move_data&)>;

			// binds action(s) to a mouse key
			static auto bind(const mouse_input&, const action&) -> void;
			static auto bind(const mouse_input&, const std::vector<action>&) -> void;

			// unbinds all actions from a mouse key
			static auto unbind(const mouse_input&) -> void;

			// unbinds a specific action from a mouse key
			static auto unbind(const mouse_input&, const action&) -> void;

			static auto move_callback(const on_move_action_t&) -> void;

		private:
			static window* s_window;
			static double s_previous_x;
			static double s_previous_y;

			static inline auto s_key_bindings =
				std::unordered_multimap<const mouse_input, const action, const mouse_input> {};
		};

		auto read_text_file(const std::filesystem::path&) -> string::string_t;
		auto read_binary_file(const std::filesystem::path&) -> std::vector<std::byte>;

		export template <file_type type = file_type::text>
		auto read_file(const std::filesystem::path& file_path)
		{
			if constexpr (type == file_type::text)
				return read_text_file(file_path);

			if constexpr (type == file_type::binary)
				return read_binary_file(file_path);
		}

		export auto initialize(const window&) -> void;

		auto assert_path_validity(const std::filesystem::path&, const file_type&) -> bool;


		export inline const auto m_valid_file_extensions = std::map<file_type, const std::vector<const char*>> {
			{file_type::text, {"txt"}}};
	};
}
