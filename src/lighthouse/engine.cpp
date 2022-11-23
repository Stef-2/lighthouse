#include "engine.hpp"

auto lh::engine::initialize(window& window) -> void
{
    static auto engine_initialized = bool {};

    if (engine_initialized)
        return;

    m_window = &window;
    bind_keyboard_input();
    bind_mouse_input();

    engine_initialized = true;
}

auto lh::engine::run() -> void
{
    while (!m_window->vkfw_window().shouldClose().value)
    {
        vkfw::pollEvents();
    }
}

auto lh::engine::bind_keyboard_input() -> void
{
    auto& keybindings = input::key_binding::get_key_bindings();

    m_window->vkfw_window().callbacks()->on_key = [&keybindings](vkfw::Window const&, vkfw::Key key, int32_t code,
                                                                 vkfw::KeyAction action,
                                                                 vkfw::ModifierKeyFlags modifiers)
    {
        auto input = input::key_binding::key_input {key, modifiers, action};
        auto iterator = keybindings.equal_range(input);

        for (auto function = iterator.first; function != iterator.second; ++function)
            function->second();
    };
}

auto lh::engine::bind_mouse_input() -> void
{
    auto& keybindings = input::key_binding::get_key_bindings();

    m_window->vkfw_window().callbacks()->on_mouse_button = [&keybindings](vkfw::Window const&, vkfw::MouseButton button,
                                                                          vkfw::MouseButtonAction action,
                                                                          vkfw::ModifierKeyFlags modifiers)
    {
        auto input = input::key_binding::key_input {button, modifiers, action};
        auto iterator = keybindings.equal_range(input);

        for (auto function = iterator.first; function != iterator.second; ++function)
            function->second();
    };
}
