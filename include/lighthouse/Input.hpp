#pragma once

#include "vkfw.hpp"

#include "window.hpp"
#include "static.hpp"

#include <functional>
#include <unordered_map>
#include <concepts>
#include <variant>
#include <vector>

namespace lh
{



    // class encapsulating functions with unique IDs
    class action
    {
    public:
        // internal function type
        using function_t = std::function<void()>;
        // internal guid counter type
        using counter_t = uint64_t;

        // make constructible from invocable / callable concepts
        action(const std::invocable auto& function) 
            : m_action(function), m_guid(counter) 
            { counter++; }

        auto get_id() const -> counter_t;

        // overlaoded () operator to call the wrapped function
        auto operator()() -> void;
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

        class key_binding
        {
        public:
            // full key input, constructed from:
            // (1) keyboard or mouse key code -> defaults to unknown
            // (2) keyboard modifier keys (ctrl, shift, alt, etc) -> defaults to no mods
            // (3) key action (press, release, hold) -> defaults to press
            struct key_input
            {
                key_input(std::variant<vkfw::Key, vkfw::MouseButton> = vkfw::Key::Unknown,
                          vkfw::ModifierKeyFlags = vkfw::ModifierKeyFlags {},
                          vkfw::KeyAction = vkfw::KeyAction::Press);

                // define comparison and hash functions so it can be used as a map key
                auto operator<=>(const key_input&) const = default;
                auto operator()(const key_input& value) const -> std::size_t;

                // allow either keyboard or mouse key
                std::variant<vkfw::Key, vkfw::MouseButton> m_key;
                vkfw::ModifierKeyFlags m_flags;
                vkfw::KeyAction m_action;
            };

            static auto get_key_bindings() -> std::unordered_multimap<key_input, action, key_input>&;

            // binds action(s) to a key
            static auto bind(const key_input&, const action&) -> void;
            static auto bind(const key_input&, const std::vector<action>&) -> void;

            // unbinds all actions from a key
            static auto unbind(const key_input&) -> void;

            // unbinds a specific action from a key
            static auto unbind(const key_input&, const action&) -> void;

        private:
            // map key inputs to any number of actions
            static inline auto m_key_bindings = std::unordered_multimap<key_input, action, key_input> {};
        };
    };
}


