#pragma once

namespace lh
{
    class static_t
    {
      public:
        static_t() = delete;
        static_t(const static_t&) = delete;
        static_t& operator=(const static_t&) = delete;
    };
}