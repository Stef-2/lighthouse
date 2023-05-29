#include "lighthouse/output.hpp"
#include "lighthouse/filesystem.hpp"

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

auto lh::output::fatal() -> buffer&
{
	m_fatal_flag = true;
	return m_error;
}

auto lh::output::initialize() -> void
{
	std::ios::sync_with_stdio(false);
}

auto lh::output::dump_logs(std::ostream& stream) -> void
{
	if (not m_log.data().empty())
		stream << "\n======== program log: ========\n" << m_log;
	if (not m_warning.data().empty())
		stream << "\n======== program warning: ========\n" << m_warning;
	if (not m_error.data().empty())
		stream << "\n======== program error: ========\n" << m_error;
}

auto lh::output::exit() -> void
{
	dump_logs(std::cout);

	std::exit(0xDEAD);
}

auto lh::output::buffer::data() const -> std::string_view
{
	return m_buffer;
}

auto lh::output::buffer::last_line() const -> std::string_view
{
	auto view = std::string_view {m_buffer};

	return view.substr(m_buffer.rfind('\n', m_buffer.size() - 2) + 1);
}

lh::output::buffer::operator lh::string::string_t() const
{
	return m_buffer;
}

auto lh::operator<<(std::ostream& stream, output::buffer& buffer) -> std::ostream&
{
	return stream << std::string {buffer};
}
