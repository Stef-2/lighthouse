#if INTELLISENSE
#include "vkfw/vkfw.hpp"

#include <variant>
#include <iostream>
#include <fstream>
#include <ranges>
#endif

module input;

import output;

namespace lh
{
	action::counter_t action::s_counter = {};
	window* input::mouse::s_window = {};
	double input::mouse::s_previous_x = {};
	double input::mouse::s_previous_y = {};

	auto action::id() const -> const counter_t&
	{
		return m_guid;
	}

	auto action::operator()() const -> void
	{
		m_action();
	}

	auto action::operator==(const action& other) const -> bool
	{
		return m_guid == other.m_guid;
	}

	auto input::key_binding::key_bindings() -> std::unordered_multimap<const key_input, const action, const key_input>&
	{
		return s_key_bindings;
	}

	auto input::key_binding::bind(const key_input& key, const action& action) -> void
	{
		s_key_bindings.insert({key, action});
	}

	auto input::key_binding::bind(const key_input& key, const std::vector<action>& actions) -> void
	{
		for (const auto& action : actions)
			bind(key, action);
	}

	auto input::key_binding::unbind(const key_input& key) -> void
	{
		if (s_key_bindings.contains(key))
			s_key_bindings.erase(key);
	}

	auto input::key_binding::unbind(const key_input& key, const action& func) -> void
	{
		if (s_key_bindings.contains(key))
		{
			auto range = s_key_bindings.equal_range(key);

			for (auto it = range.first; it != range.second; ++it)
				if (it->second == func)
					s_key_bindings.erase(it);
		}
	}

	auto input::key_binding::initialize(const window& window) -> void
	{
		const auto& key_bindings = s_key_bindings;
		auto& pressed_keys = s_pressed_keys;

		// bind keyboard buttons
		window.vkfw_window().callbacks()->on_key = [&key_bindings, &pressed_keys](vkfw::Window const&,
																				  vkfw::Key key,
																				  int32_t code,
																				  vkfw::KeyAction action,
																				  vkfw::ModifierKeyFlags modifiers) {
			if (action == vkfw::KeyAction::Press)
				pressed_keys.emplace_back(key, modifiers, action);
			else if (action == vkfw::KeyAction::Release)
				pressed_keys.erase(std::remove(pressed_keys.begin(),
											   pressed_keys.end(),
											   key_input {key, modifiers, vkfw::KeyAction::Press}));
		};

		// bind mouse buttons
		window.vkfw_window().callbacks()->on_mouse_button = [&key_bindings,
															 &pressed_keys](vkfw::Window const&,
																			vkfw::MouseButton key,
																			vkfw::MouseButtonAction action,
																			vkfw::ModifierKeyFlags modifiers) {
			if (action == vkfw::MouseButtonAction::Press)
				pressed_keys.emplace_back(key, modifiers, action);
			else if (action == vkfw::MouseButtonAction::Release)
				pressed_keys.erase(std::remove(pressed_keys.begin(),
											   pressed_keys.end(),
											   key_input {key, modifiers, vkfw::KeyAction::Press}));
		};
	}

	auto input::key_binding::execute_pressed_keys() -> void
	{
		for (const auto& [map_key, value] : s_key_bindings)
			for (const auto& pressed_key : s_pressed_keys)
				if (map_key == pressed_key)
					value();
	}

	auto input::mouse::initialize(const window& window) -> void
	{
		s_window = &const_cast<lh::window&>(window);

		window.vkfw_window().setCursorPos(window.resolution().width / 2, window.resolution().height / 2);
		s_previous_x = window.resolution().width / 2;
		s_previous_y = window.resolution().height / 2;
	}

	auto input::mouse::move_callback(const on_move_action_t& action) -> void
	{
		s_window->vkfw_window().callbacks()->on_cursor_move = [action](vkfw::Window const&, double x, double y) {
			action(move_info {{x, y}, {s_previous_x, s_previous_y}});

			s_previous_x = x;
			s_previous_y = y;
		};
	}

	auto input::read_text_file(const std::filesystem::path& file_path) -> string::string_t
	{
		if (not assert_path_validity(file_path, file_type::text))
			return {};

		auto stream = std::ifstream {file_path, std::ios::in};
		auto buffer = std::stringstream {};
		buffer << stream.rdbuf();
		stream.close();

		return buffer.str();
	}

	auto input::read_binary_file(const std::filesystem::path& file_path) -> std::vector<std::byte>
	{
		if (not assert_path_validity(file_path, file_type::binary))
			return {};

		auto stream = std::ifstream {file_path, std::ios::in | std::ios::binary | std::ios::ate};
		const auto file_size = stream.tellg();
		stream.seekg(std::ios::beg);

		auto buffer = std::vector<std::byte>(file_size);
		stream.read(reinterpret_cast<char*>(buffer.data()), file_size);

		stream.close();

		return buffer;
	}

	auto input::assert_path_validity(const std::filesystem::path& file_path, const file_type& file_type) -> bool
	{
		const auto valid_path = not file_path.empty();
		const auto valid_extension = std::ranges::any_of(m_valid_file_extensions.at(file_type),
														 [&file_path](const auto& x) {
															 return x == file_path.extension();
														 });

		if (valid_path and valid_extension)
		{
			output::error() << "invalid file path provided: " + file_path.string();
			return false;
		}

		return true;
	}

	input::key_binding::key_input::key_input(std::variant<vkfw::Key, vkfw::MouseButton> key,
											 vkfw::ModifierKeyFlags flags,
											 std::variant<vkfw::KeyAction, vkfw::MouseButtonAction> action)
		: m_key(std::visit([](auto& x) { return std::to_underlying(x); }, key)),
		  m_modifier_flags(flags),
		  m_action(std::visit([](auto& x) { return std::to_underlying(x); }, action))
	{}

	auto input::key_binding::key_input::operator()(const key_input& value) const -> std::size_t
	{
		return m_key + m_modifier_flags + m_action;
	}

	auto input::initialize(const window& window) -> void
	{
		key_binding::initialize(window);
		mouse::initialize(window);
	}
}
