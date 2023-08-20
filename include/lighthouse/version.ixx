module;

#if INTELLISENSE
#include <cstdint>
#include <compare>
#endif

export module version;

import lighthouse_string;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	struct version
	{
		using version_t = std::uint8_t;
		using packed_version_t = std::uint32_t;

		version(version_t major, version_t minor, version_t patch);
		// extract the 3 8-bit versions from a packed 32-bit one
		version(packed_version_t);

		auto major() const -> version_t;
		auto minor() const -> version_t;
		auto patch() const -> version_t;

		// implicit conversions to wider type, packs all 3 versions
		operator packed_version_t() const;

		// implicit conversion to a readable string format
		operator string::string_t() const;

		auto operator<=>(const version&) const = default;

	private:
		packed_version_t m_version {};

		static inline constexpr auto s_pack_offset = 0xFF;
	};
}
