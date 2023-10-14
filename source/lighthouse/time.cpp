#if INTELLISENSE
#include <chrono>
#endif

module time;

namespace lh
{
	namespace time
	{
		time_point_t s_engine_start_time = clock_t::now();
		time_point_t s_previous_frame_time = clock_t::now();

		stopwatch::stopwatch() : m_start_time {time_point_t {}}
		{
			start();
		}

		auto stopwatch::start() -> void
		{
			m_start_time = time::clock_t::now();
		}

		auto stopwatch::stop() -> const time_duration_t
		{
			return std::chrono::duration_cast<precision_t>(clock_t::now() - m_start_time).count();
		}

		auto engine_start_time() -> const time_point_t&
		{
			return s_engine_start_time;
		}

		auto time_between(const time_point_t& x, const time_point_t& y) -> const duration_t
		{
			return duration_t {x - y};
		}

		auto delta_time() -> const floating_time_t
		{
			const auto now = clock_t::now();
			const auto delta_time = std::chrono::duration<floating_time_t>(now - s_previous_frame_time);
			s_previous_frame_time = now;

			return delta_time.count();
		}
	}
}
