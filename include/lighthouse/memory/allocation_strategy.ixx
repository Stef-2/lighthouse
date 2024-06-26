export module allocation_strategy;

export namespace lh
{
	enum class allocation_strategy
	{
		first_fit,
		best_fit,
		worst_fit,
		default_strategy = first_fit
	};
}