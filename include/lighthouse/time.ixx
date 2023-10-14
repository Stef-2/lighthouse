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
		export using duration_t = std::chrono::duration<precision_t>;
		export using time_point_t = std::chrono::time_point<clock_t>;
		export using time_duration_t = std::uint64_t;
		export using floating_time_t = double;

		template <typename T>
		concept time_conversion = std::is_same_v<T, floating_time_t> or std::is_same_v<T, time_duration_t> or
								  std::is_same_v<T, time_point_t>;

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

		//export template <time_conversion T = floating_time_t>
		auto now() -> double
		{
			const auto time = clock_t::now().time_since_epoch().count() - clock_t::now().time_since_epoch().count()/*engine_start_time()*/;
			
			//if constexpr (std::is_same_v<T, floating_time_t>)
			{
				//const auto wtf = std::chrono::duration_cast<floating_time_t>(time);
				return time / 1000;
			}
			
			/*
			if constexpr (std::is_same_v<T, time_duration_t>)
				return std::chrono::duration_cast<precision_t>(time).count();*/
			/*
			if constexpr (std::is_same_v<T, time_point_t>)
				return time;*/
		}
	}
}