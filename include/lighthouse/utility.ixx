module;

export module lighthouse_utility;

import std;

export namespace lh
{
	namespace concepts
	{
		template <typename T, typename... Ts>
		concept is_any = std::disjunction_v<std::is_same<T, Ts>...>;

		template <typename T, typename... Ts>
		concept are_same = std::conjunction_v<std::is_same<T, Ts>...>;
	}

	template <typename... Ts>
	struct variadic
	{
		template <typename... Ys>
		static consteval auto match()
		{
			return std::is_same_v<std::tuple<Ts...>, std::tuple<Ys...>>;
		}
	};

	template <typename... Ts>
	struct dependent_false : std::false_type
	{};

	namespace function_parameters
	{
		template <typename T>
		struct function_traits;

		template <typename R, typename... As>
		class function_traits<R(As...)>
		{
		public:
			using arguments = ::std::tuple<As...>;

			template <typename... Ts>
			static consteval auto match()
			{
				return std::is_same_v<arguments, std::tuple<Ts...>>;
			}
		};
	}

	template <typename T, std::size_t N = std::dynamic_extent>
	using static_or_dynamic = std::conditional<N == std::dynamic_extent, std::vector<T>, std::array<T, N>>;

	template <typename T>
	using non_owning_ptr = T*;

	struct empty {};

	struct storage
	{
		void* m_data;
		std::size_t m_size;
	};
}