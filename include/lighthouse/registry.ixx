module;

export module registry;

import lighthouse_utility;

import std;

export namespace lh
{
	constexpr auto initial_registry_capacity = 0;

	// forward declaration
	template <typename T>
	class registry_entry;

	// to be inherited by a type that is to act as a registry for type T
	// keeps track of objects of type T
	template <typename T, std::size_t N = initial_registry_capacity>
	class registry
	{
	public:
		using entries_t = std::vector<non_owning_ptr<T>>;
		friend registry_entry<T>;

		registry() : m_entries {}
		{
			if constexpr (N > 0) m_entries.reserve(N);

			registry_entry<T>::s_registry = this;
		}

		registry(const registry&) = delete;
		auto operator=(const registry&) -> registry& = delete;

		auto entries() const -> const entries_t& { return m_entries; }

	protected:
		entries_t m_entries {};
	};

	// CRTP to be inherited by a type T whose objects are to be registered in registry<T>
	// objects of type T inheriting this one may only be created after an object of registry<T> has been created
	template <typename T>
	class registry_entry
	{
	public:
		friend registry<T>;

	protected:
		registry_entry() { s_registry->m_entries.push_back(static_cast<non_owning_ptr<T>>(this)); }
		~registry_entry() { std::erase(s_registry->m_entries, static_cast<non_owning_ptr<T>>(this)); }

		registry_entry(const registry_entry& other) = delete;
		auto operator=(const registry_entry& other) -> registry_entry& = delete;

		registry_entry(registry_entry&& other)
		{
			const auto entry = std::ranges::find(s_registry->m_entries, static_cast<non_owning_ptr<T>>(this));
			*entry = static_cast<non_owning_ptr<T>>(this);
		}

		auto operator=(registry_entry&& other) -> registry_entry&
		{
			const auto entry = std::ranges::find(s_registry->m_entries, static_cast<non_owning_ptr<T>>(this));
			*entry = static_cast<non_owning_ptr<T>>(this);

			return *this;
		}

		static inline non_owning_ptr<registry<T>> s_registry {nullptr};
	};
}