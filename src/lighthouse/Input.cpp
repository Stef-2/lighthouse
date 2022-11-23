#include "input.hpp"

auto lh::action::get_id() const -> counter_t
{
    return m_guid;
}

auto lh::action::operator()() -> void
{
    m_action();
}

auto lh::action::operator==(const action& other) const -> bool
{
    return m_guid == other.m_guid;
}

auto lh::input::key_binding::get_key_bindings() -> std::unordered_multimap<key_input, action, key_input>&
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
                                vkfw::KeyAction action)
    : m_key(key), m_flags(flags), m_action(action)
{
}

auto lh::input::key_binding::key_input::operator()(const key_input& value) const -> std::size_t
{
    // fake boost multihash
    auto seed = std::size_t {0};

    auto hash = std::hash<int>();

    auto key_variant = std::holds_alternative<vkfw::Key>(value.m_key);
    auto key = key_variant ? std::to_underlying(std::get<vkfw::Key>(value.m_key))
                           : std::to_underlying(std::get<vkfw::MouseButton>(value.m_key));
    auto flags = static_cast<int>(value.m_flags);
    auto action = std::to_underlying(value.m_action);

    seed ^= hash(key) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash(flags) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash(action) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    return seed;
}
