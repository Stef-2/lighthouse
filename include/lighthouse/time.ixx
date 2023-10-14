module;

#if INTELLISENSE
#include <chrono>
#include <concepts>
#endif

export module time;

#if not INTELLISENSE
import std.core;
#endif

namespace lh
{
	namespace time
	{
		export using clock_t = std::chrono::steady_clock;
		export using precision_t = std::chrono::milliseconds;
		export using duration_t = precision_t;
		export using time_point_t = std::chrono::time_point<clock_t>;
		export using time_duration_t = std::uint64_t;
		export using floating_time_t = double;

		export class stopwatch
		{
		public:
			stopwatch();

			auto start() -> void;
			auto stop() -> const time_duration_t;
		private:
			time_point_t m_start_time;
		};
		
		export auto engine_start_time() -> const time_point_t&;
		export auto time_between(const time_point_t&, const time_point_t&) -> const duration_t;
		export auto delta_time() -> const floating_time_t;

		export auto now() -> floating_time_t
		{
			return std::chrono::duration<floating_time_t>(clock_t::now() - engine_start_time()).count();
		}
	}
}