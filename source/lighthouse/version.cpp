#include "lighthouse/version.hpp"

lh::version::operator lh::string::string_t() const
{
	return std::to_string(major()) + ':' + std::to_string(minor()) + ':' + std::to_string(patch());
}

lh::version::version(version_t major, version_t minor, version_t patch)
	: m_version(VK_MAKE_VERSION(major, minor, patch))
{}

auto lh::version::major() const -> version_t
{
	return VK_API_VERSION_MAJOR(m_version);
}

auto lh::version::minor() const -> version_t
{
	return VK_API_VERSION_MINOR(m_version);
}

auto lh::version::patch() const -> version_t
{
	return VK_API_VERSION_PATCH(m_version);
}

lh::version::version(packed_version_t version) : m_version(version) {}

lh::version::operator packed_version_t() const
{
	return m_version;
}
