module;

export module registry;

import std;
import lighthouse_utility;

namespace lh
{
	constexpr auto connection_to_registry = false;

	template <typename T, bool R>
	class registry_item;

	template <typename T, bool R = connection_to_registry>
	class registry
	{
	public:
		friend registry_item<T, R>;

		registry()
		{
			if constexpr (R)
				T::m_registry = this;
		}

	protected:
		static std::vector<T*> m_entries;
	};

	template <typename T, bool R = connection_to_registry>
	class registry_item
	{
	public:
		friend registry<T, R>;

	protected:
		registry_item() { registry<T>::m_entries.push_back(static_cast<T*>(this)); }
		static registry<T, R>* m_registry;
	};

	template <typename T, bool R = connection_to_registry>
	class full_registry : public registry_item<T, R>, public registry<T, R>
	{};

	class item : public full_registry<item>
	{};

	void func()
	{
		// registry_item r ();
		registry<item> reg;

		item i;
	}
}

export namespace lh
{

}