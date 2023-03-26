#pragma once

#include "vkfw.hpp"

#include "static.hpp"

#include <unordered_map>
#include <variant>

namespace lh
{
	// forward declarations
	class window;

	// class encapsulating functions with unique IDs
	class action
	{
	public:
		// internal function type
		using function_t = std::function<void()>;
		// internal guid counter type
		using counter_t = uint64_t;

		// make constructible from invocable / callable concepts
		action(const std::invocable auto& function) : m_action(function), m_guid(counter) { counter++; }

		auto get_id() const -> counter_t;

		// overlaoded () operator to call the wrapped function
		auto operator()() const -> void;
		// comparison operators for mapping
		auto operator==(const action&) const -> bool;
		auto operator<=>(const action&) const = default;

	private:
		// global counter guid counter
		static inline auto counter = counter_t {};

		function_t m_action;
		counter_t m_guid;
	};

	class input : static_t
	{
	public:
		friend class engine;

		class key_binding
		{
		public:
			friend class input;

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
				int m_key;
				int m_flags;
				int m_action;
			};

			static auto get_key_bindings() -> std::unordered_multimap<const key_input, const action, const key_input>&;

			// binds action(s) to a key
			static auto bind(const key_input&, const action&) -> void;
			static auto bind(const key_input&, const std::vector<action>&) -> void;

			// unbinds all actions from a key
			static auto unbind(const key_input&) -> void;

			// unbinds a specific action from a key
			static auto unbind(const key_input&, const action&) -> void;

		private:
			static auto initialize(const window&) -> void;

			// map key inputs to any number of actions
			static inline auto m_key_bindings =
				std::unordered_multimap<const key_input, const action, const key_input> {};
		};

	private:
		static auto initialize(const window&) -> void;
	};
}
