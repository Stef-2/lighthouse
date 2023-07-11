module;

#define SYSTEM unknown;

#ifdef _WIN32
	#define SYSTEM windows;
#endif

#ifdef linux
	#define SYSTEM linux;
#endif

export module system;

export namespace lh
{
	class system
	{
	public:
		enum class type
		{
			windows,
			linux
		};

		static inline constexpr auto type = type::SYSTEM;
	};
}