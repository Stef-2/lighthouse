#pragma once

#include <iostream>
#include <string>
#include <string_view>

namespace lh
{
    // static utility class that provides custom logging facilities
    class Output
    {
      public:
        // string type to be used as the internal buffer
        using string = std::string;

        // static class, prevent construction
        Output() = delete;
        Output(const Output&) = delete;
        Output& operator=(const Output&) = delete;

        // custom buffer
        class Buffer
        {
          public:
            auto get_data() const -> std::string_view;
            auto get_last_line() const -> std::string_view;

            // enable std::cout like input
            auto operator<<(std::string_view) -> Buffer&;

            // implicit string conversion
            operator string() const;

          private:
            string m_buffer {};
        };

        static auto log() -> Buffer&;
        static auto warning() -> Buffer&;
        static auto error() -> Buffer&;

      private:
        static Buffer m_log;
        static Buffer m_warning;
        static Buffer m_error;
    };

    auto operator<<(std::ostream& stream, lh::Output::Buffer& buffer) -> std::ostream&;
}
