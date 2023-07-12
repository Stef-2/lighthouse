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
	namespace system
	{
		enum class type
		{
			windows,
			linux
		};

		constexpr auto type = type::SYSTEM;
	};
}