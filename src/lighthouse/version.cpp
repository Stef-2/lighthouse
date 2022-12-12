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
    : m_major(version >> 16 & m_pack_offset), m_minor(version >> 8 & m_pack_offset), m_patch(version & m_pack_offset)
{
}

lh::engine_version::operator packed_version_t() const
{
    return m_major << 16 | m_minor << 8 | m_patch;
}


