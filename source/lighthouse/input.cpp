#include <functional>

module input;

import output;

namespace lh
{
	action::counter_t action::s_counter = {};
	window* input::mouse::s_window = {};
	double input::mouse::s_previous_x = {};
	double input::mouse::s_previous_y = {};

	auto action::get_id() const -> const counter_t&
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

	auto input::keyboard::key_bindings() -> std::unordered_multimap<const key_input, const action, const key_input>&
	{
		return s_key_bindings;
	}

	auto input::keyboard::bind(const key_input& key, const action& action) -> void
	{
		s_key_bindings.insert({key, action});
	}

	auto input::keyboard::bind(const key_input& key, const std::vector<action>& actions) -> void
	{
		for (const auto& action : actions)
			bind(key, action);
	}

	auto input::keyboard::unbind(const key_input& key) -> void
	{
		if (s_key_bindings.contains(key))
			s_key_bindings.erase(key);
	}

	auto input::keyboard::unbind(const key_input& key, const action& func) -> void
	{
		if (s_key_bindings.contains(key))
		{
			auto range = s_key_bindings.equal_range(key);

			for (auto it = range.first; it != range.second; ++it)
				if (it->second == func)
					s_key_bindings.erase(it);
		}
	}

	auto input::keyboard::initialize(const window& window) -> void
	{
		const auto& keybindings = s_key_bindings;

		// bind keyboard
		window.vkfw_window().callbacks()->on_key = [&keybindings](vkfw::Window const&,
																  vkfw::Key key,
																  int32_t code,
																  vkfw::KeyAction action,
																  vkfw::ModifierKeyFlags modifiers) {
			const auto input = input::keyboard::key_input {key, modifiers, action};
			auto iterator = keybindings.equal_range(input);

			for (auto& function = iterator.first; function != iterator.second; function++)
				function->second();
		};
	}

	auto input::mouse::initialize(const window& window) -> void
	{
		const auto& keybindings = s_key_bindings;

		s_window = &const_cast<lh::window&>(window);

		window.vkfw_window().setCursorPos(window.resolution().width / 2, window.resolution().height / 2);
		s_previous_x = window.resolution().width / 2;
		s_previous_y = window.resolution().height / 2;

		// bind mouse
		window.vkfw_window().callbacks()->on_mouse_button = [&keybindings](vkfw::Window const&,
																		   vkfw::MouseButton button,
																		   vkfw::MouseButtonAction action,
																		   vkfw::ModifierKeyFlags modifiers) {
			const auto input = input::mouse::mouse_input {button, modifiers, action};
			auto iterator = keybindings.equal_range(input);

			for (auto function = iterator.first; function != iterator.second; ++function)
				function->second();
		};
	}

	auto input::mouse::move_callback(const on_move_action_t& action) -> void
	{
		s_window->vkfw_window().callbacks()->on_cursor_move = [action](vkfw::Window const&, double x, double y) {
			action(move_data {{x, y}, {s_previous_x, s_previous_y}});

			s_previous_x = x;
			s_previous_y = y;
		};
	}

	auto input::mouse::bind(const mouse_input& key, const action& action) -> void
	{
		s_key_bindings.insert({key, action});
	}

	auto input::mouse::bind(const mouse_input& key, const std::vector<action>& actions) -> void
	{
		for (const auto& action : actions)
			bind(key, action);
	}

	auto input::mouse::unbind(const mouse_input& key) -> void
	{
		if (s_key_bindings.contains(key))
			s_key_bindings.erase(key);
	}

	auto input::mouse::unbind(const mouse_input& key, const action& func) -> void
	{
		if (s_key_bindings.contains(key))
		{
			auto range = s_key_bindings.equal_range(key);

			for (auto it = range.first; it != range.second; ++it)
				if (it->second == func)
					s_key_bindings.erase(it);
		}
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

	input::keyboard::key_input::key_input(vkfw::Key key, vkfw::ModifierKeyFlags flags, vkfw::KeyAction action)
		: m_key {key}, m_flags {flags}, m_action {action}
	{}

	input::mouse::mouse_input::mouse_input(vkfw::MouseButton key,
										   vkfw::ModifierKeyFlags flags,
										   vkfw::MouseButtonAction action)
		: m_key {key}, m_flags {flags}, m_action {action}
	{}

	auto input::keyboard::key_input::operator()(const key_input& value) const -> std::size_t
	{
		return 0;
	}

	auto input::mouse::mouse_input::operator()(const mouse_input& value) const -> std::size_t
	{
		return 0;
	}

	auto input::initialize(const window& window) -> void
	{
		keyboard::initialize(window);
		mouse::initialize(window);
	}
}
