module;

module output;

namespace lh
{
	auto output::log() -> buffer&
	{
		return n_log;
	}

	auto output::warning() -> buffer&
	{
		return n_warning;
	}

	auto output::error() -> buffer&
	{
		return n_error;
	}

	auto output::fatal() -> buffer&
	{
		n_fatal_flag = true;
		return n_error;
	}

	auto output::initialize() -> void
	{
		std::ios::sync_with_stdio(false);
	}

	auto output::dump_logs(std::ostream& stream) -> void
	{
		if (not n_log.data().empty())
			stream << "\n======== program log: ========\n" << n_log;
		if (not n_warning.data().empty())
			stream << "\n======== program warning: ========\n" << n_warning;
		if (not n_error.data().empty())
			stream << "\n======== program error: ========\n" << n_error;
	}

	auto output::exit() -> void
	{
		dump_logs(std::cout);

		std::exit(0xDEAD);
	}

	auto output::buffer::data() const -> std::string_view
	{
		return m_buffer;
	}

	auto output::buffer::last_line() const -> std::string_view
	{
		auto view = std::string_view {m_buffer};

		return view.substr(m_buffer.rfind('\n', m_buffer.size() - 2) + 1);
	}

	output::buffer::operator string::string_t() const
	{
		return m_buffer;
	}
}

auto operator<<(std::ostream& stream, lh::output::buffer& buffer) -> std::ostream&
{
	return stream << std::string {buffer};
}
