#include "version.hpp"

const lh::engine_version lh::engine_version::m_default = {0, 2, 0};
const lh::vulkan_version lh::vulkan_version::m_default = {1, 3, 0};

lh::engine_version::operator std::string() const
{
    return "engine version: " + m_major + ':' + m_minor + ':' + m_patch;
}

lh::engine_version::engine_version(version_t major, version_t minor, version_t patch)
    : m_major(major), m_minor(minor), m_patch(patch)
{
}

lh::engine_version::engine_version(packed_version_t version)
    : m_major(version >> 16 & 0xFF), m_minor(version >> 8 & 0xFF), m_patch(version & 0xFF)
{
}

lh::engine_version::operator packed_version_t() const
{
    return m_major << 16 | m_minor << 8 | m_patch;
}


