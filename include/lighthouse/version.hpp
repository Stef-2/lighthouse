#pragma once

#include "lighthouse/string.hpp"

namespace lh
{
	struct version
	{
		using version_t = uint8_t;
		using packed_version_t = uint32_t;

		version(version_t major, version_t minor, version_t patch);
		// extract the 3 8-bit versions from a packed 32-bit one
		version(packed_version_t);

		auto major() const -> version_t;
		auto minor() const -> version_t;
		auto patch() const -> version_t;

		// implicit conversions to wider type, packs all 3 versions
		operator packed_version_t() const;

		// implicit conversion to a readable string format
		operator lh::string::string_t() const;

		static const version m_engine_version;
		static const version m_vulkan_version;

	private:
		packed_version_t m_version {};
		static inline int m_pack_offset = 0xFF;
	};
}
