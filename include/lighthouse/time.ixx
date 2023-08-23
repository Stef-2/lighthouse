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
		export using clock_t = std::chrono::high_resolution_clock;
		export using time_point_t = std::chrono::time_point<clock_t>;
		export using precision_t = std::chrono::milliseconds;
		export using time_duration_t = std::uint64_t;
		export using delta_time_t = double;

		template <typename T>
		concept time_conversion = std::same_as<T, double> || std::same_as<T, time_duration_t> ||
								  std::same_as<T, time_point_t>;

		export class stopwatch
		{
		public:
			stopwatch();

			auto start() -> void;
			auto stop() -> const time_duration_t;
		private:
			time_point_t m_start_time;
		};

		export template <time_conversion T = time_duration_t>
		auto now() -> auto
		{
			if constexpr (std::same_as<T, double> || std::same_as<T, time_duration_t>)
				return std::chrono::duration<T>(clock_t::now().time_since_epoch().count());

			if constexpr (std::same_as<T, time_point_t>)
				return clock_t::now();

		}

		export auto engine_start_time() -> const time_point_t&;
		export auto time_between(const time_point_t&, const time_point_t&) -> const time_duration_t;
		export auto delta_time() -> const delta_time_t;

		static time_point_t s_engine_start_time;
		static time_point_t s_previous_frame_time;
	}
}