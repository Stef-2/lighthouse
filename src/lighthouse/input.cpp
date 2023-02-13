#include "input.hpp"

auto lh::action::get_id() const -> counter_t
{
    return m_guid;
}

auto lh::action::operator()() const -> void
{
    m_action();
}

auto lh::action::operator==(const action& other) const -> bool
{
    return m_guid == other.m_guid;
}

auto lh::input::key_binding::get_key_bindings()
    -> std::unordered_multimap<const key_input, const action, const key_input>&
{
    return m_key_bindings;
}

auto lh::input::key_binding::bind(const key_input& key, const action& action) -> void
{
    m_key_bindings.insert({key, action});
}

auto lh::input::key_binding::bind(const key_input& key, const std::vector<action>& actions) -> void
{
    for (const auto& action : actions)
        bind(key, action);
}

auto lh::input::key_binding::unbind(const key_input& key) -> void
{
    if (m_key_bindings.contains(key))
        m_key_bindings.erase(key);
}

auto lh::input::key_binding::initialize(const window& window) -> void
{
    auto& keybindings = m_key_bindings;
    
    // bind keyboard
    window.vkfw_window().callbacks()->on_key = [&keybindings](vkfw::Window const&, vkfw::Key key, int32_t code,
                                                                 vkfw::KeyAction action,
                                                                 vkfw::ModifierKeyFlags modifiers) {
        const auto input = input::key_binding::key_input {key, modifiers, action};
        auto iterator = keybindings.equal_range(input);
        std::cout << code;
        for (auto& function = iterator.first; function != iterator.second; function++)
            function->second();
    };

    // bind mouse
    window.vkfw_window().callbacks()->on_mouse_button = [&keybindings](vkfw::Window const&, vkfw::MouseButton button,
                                                                          vkfw::MouseButtonAction action,
                                                                          vkfw::ModifierKeyFlags modifiers) {
        const auto input = input::key_binding::key_input {button, modifiers, action};
        auto iterator = keybindings.equal_range(input);

        for (auto function = iterator.first; function != iterator.second; ++function)
            function->second();
    };

}

auto lh::input::key_binding::unbind(const key_input& key, const action& func) -> void
{
    if (m_key_bindings.contains(key))
    {
        auto range = m_key_bindings.equal_range(key);

        for (auto it = range.first; it != range.second; ++it)
            if (it->second == func)
                m_key_bindings.erase(it);
    }
}

lh::input::key_binding::key_input::key_input(std::variant<vkfw::Key, vkfw::MouseButton> key,
                          vkfw::ModifierKeyFlags flags,
                          std::variant<vkfw::KeyAction, vkfw::MouseButtonAction> action)
    : m_key(std::visit([](auto& x){return std::to_underlying(x);}, key)),
    m_flags(flags),
    m_action(std::visit([](auto& x) {return std::to_underlying(x);}, action))
{
}

auto lh::input::key_binding::key_input::operator()(const key_input& value) const -> std::size_t
{
    return 0;
}

auto lh::input::initialize(const window& window) -> void
{
    key_binding::initialize(window);
}
