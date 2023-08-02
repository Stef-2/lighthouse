module;

#include "vulkan/vulkan.h"

#if INTELLISENSE
#include "lighthouse/version.ixx"
#else
module version;
#endif

namespace lh
{
	version::operator string::string_t() const
	{
		return std::to_string(major()) + ':' + std::to_string(minor()) + ':' + std::to_string(patch());
	}

	version::version(version_t major, version_t minor, version_t patch)
		: m_version(VK_MAKE_VERSION(major, minor, patch))
	{}

	auto version::major() const -> version_t
	{
		return VK_API_VERSION_MAJOR(m_version);
	}

	auto version::minor() const -> version_t
	{
		return VK_API_VERSION_MINOR(m_version);
	}

	auto version::patch() const -> version_t
	{
		return VK_API_VERSION_PATCH(m_version);
	}

	version::version(packed_version_t version) : m_version(version) {}

	version::operator packed_version_t() const
	{
		return m_version;
	}
}
