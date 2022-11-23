#include "Output.hpp"

lh::output::buffer lh::output::m_log {};
lh::output::buffer lh::output::m_warning {};
lh::output::buffer lh::output::m_error {};

auto lh::output::log() -> buffer&
{
    return m_log;
}

auto lh::output::warning() -> buffer&
{
    return m_warning;
}

auto lh::output::error() -> buffer&
{
    return m_error;
}

auto lh::output::buffer::get_data() const -> std::string_view
{
    return m_buffer;
}

auto lh::output::buffer::get_last_line() const -> std::string_view
{
    auto view = std::string_view {m_buffer};

    return view.substr(m_buffer.rfind('\n', m_buffer.size() - 2) + 1);
}

auto lh::output::buffer::operator<<(std::string_view string) -> buffer&
{
    m_buffer.append(string.data()).append("\n");

    return *this;
}

lh::output::buffer::operator lh::output::string() const
{
    return m_buffer;
}

auto lh::operator<<(std::ostream& stream, output::buffer& buffer) -> std::ostream&
{
    return stream << std::string {buffer};
}
