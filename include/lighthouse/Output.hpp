#pragma once

#include "Static.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace lh
{
    // static utility class that provides custom logging facilities
    class output : static_t
    {
      public:
        // string type to be used as the internal buffer
        using string = std::string;

        // custom buffer
        class buffer
        {
          public:
            auto get_data() const -> std::string_view;
            auto get_last_line() const -> std::string_view;

            // enable std::cout like input
            auto operator<<(std::string_view) -> buffer&;

            // implicit string conversion
            operator string() const;

          private:
            string m_buffer {};
        };

        static auto log() -> buffer&;
        static auto warning() -> buffer&;
        static auto error() -> buffer&;

      private:
        static buffer m_log;
        static buffer m_warning;
        static buffer m_error;
    };

    auto operator<<(std::ostream& stream, lh::output::buffer& buffer) -> std::ostream&;
}
