#include "Output.hpp"

lh::Output::Buffer lh::Output::m_log {};
lh::Output::Buffer lh::Output::m_warning {};
lh::Output::Buffer lh::Output::m_error {};

auto lh::Output::log() -> Buffer&
{
    return m_log;
}

auto lh::Output::warning() -> Buffer&
{
    return m_warning;
}

auto lh::Output::error() -> Buffer&
{
    return m_error;
}

auto lh::Output::Buffer::get_data() const -> std::string_view
{
    return m_buffer;
}

auto lh::Output::Buffer::get_last_line() const -> std::string_view
{
    auto view = std::string_view {m_buffer};

    return view.substr(m_buffer.rfind('\n', m_buffer.size() - 2) + 1);
}

auto lh::Output::Buffer::operator<<(std::string_view string) -> Buffer&
{
    m_buffer.append(string.data()).append("\n");

    return *this;
}

lh::Output::Buffer::operator lh::Output::string() const
{
    return m_buffer;
}

auto lh::operator<<(std::ostream& stream, Output::Buffer& buffer) -> std::ostream&
{
    return stream << std::string {buffer};
}
