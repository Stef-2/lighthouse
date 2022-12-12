#pragma once

#include <string>

namespace lh
{
    struct engine_version
    {
        using version_t = uint8_t;
        using packed_version_t = uint32_t;

        engine_version(version_t major, version_t minor, version_t patch);
        // extract the 3 8-bit versions from a packed 32-bit one
        engine_version(packed_version_t);

        // implicit conversions to wider type, packs all 3 versions
        operator packed_version_t() const;

        // implicit conversion to a readable string format
        operator std::string() const;

        version_t m_major {};
        version_t m_minor {};
        version_t m_patch {};

        static const engine_version m_default;

    private:
        static inline int m_pack_offset = 0xFF;
    };

    struct vulkan_version
    {
        uint8_t m_major;
        uint8_t m_minor;
        uint8_t m_patch;

        static const vulkan_version m_default;
    };
}
