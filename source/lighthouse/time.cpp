#if INTELLISENSE
#include <chrono>
#endif

module time;

namespace lh
{
	namespace time
	{
		time_point_t s_engine_start_time = clock_t::now();

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
			return std::chrono::duration_cast<precision_t>(clock_t::now().time_since_epoch() -
														   m_start_time.time_since_epoch())
				.count();
		}

		auto engine_start_time() -> const time_point_t&
		{
			return s_engine_start_time;
		}

		auto time_between(const time_point_t& x, const time_point_t& y) -> const time_duration_t
		{
			return std::abs(std::chrono::duration_cast<precision_t>(x - y).count());
		}
	}
}
