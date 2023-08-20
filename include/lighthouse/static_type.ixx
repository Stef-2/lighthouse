module;

export module static_type;

export namespace lh
{
	// utility class to be inherited by any other that wishes to be static
	class static_type
	{
	public:
		static_type() = delete;
		static_type(const static_type&) = delete;
		static_type& operator=(const static_type&) = delete;
	};
}
