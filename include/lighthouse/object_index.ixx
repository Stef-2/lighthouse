module;

#include <cstddef>

export module object_index;

export namespace lh
{
	template <typename T>
	class object_index
	{
	public:
		using index_t = std::size_t;

		object_index() : m_index {s_objects_created}
		{
			s_objects_created++;
			s_objects_alive++;
		}

		object_index(const object_index& other)
			: m_index {other.m_index}
		{}

		object_index& operator=(const object_index& other)
		{
			m_index = other.m_index;
		}

		object_index(object_index&& other) : m_index {other.m_index}
		{}

		object_index& operator=(object_index&& other)
		{
			m_index = other.m_index;
		}

		static auto objects_created() -> const index_t
		{
			return s_objects_created;
		}

		static auto objects_alive() -> const index_t
		{
			return s_objects_alive;
		}

		auto index() const -> const index_t
		{
			return m_index;
		}

	protected:

		// disallow destruction of this type through pointers
		~object_index()
		{
			s_objects_alive--;
		}

		static inline auto s_objects_created = index_t {};
		static inline auto s_objects_alive = index_t {};

		const index_t m_index;
	};

	// equality operator based on index comparison
	template <typename T>
	auto operator==(const object_index<T>& lhs, const object_index<T>& rhs) -> bool
	{
		return lhs.index() == rhs.index();
	}
}