module;

#if INTELLISENSE
	#include <cstddef>
	#include <vector>
	#include <array>
#endif

export module lhd_format;

#if not INTELLISENSE
import std.core;
#endif

import data_type;
import version;

export namespace lh
{
	namespace lhd
	{
		namespace layout
		{
			using checksum_t = std::byte[8];
			using magic_number_t = std::array<std::byte, 10>;
			using data_size_t = std::byte[8];

			constexpr auto file_start = magic_number_t {std::byte {0x6C},
														std::byte {0x69},
														std::byte {0x67},
														std::byte {0x68},
														std::byte {0x74},
														std::byte {0x68},
														std::byte {0x6F},
														std::byte {0x75},
														std::byte {0x73},
														std::byte {0x65}};
			
			consteval auto strlen1(const char* start)
			{
				int i = 0;

				while (start[i] != '\0')
				{
					i++;
				}

				return i;
			}

			struct stru
			{
				constexpr stru(const char* c) : cc {c}, sz {}
				{
					int i = 0;

					while (c[i] != '\0')
					{
						i++;
					}
					sz = i;
				}

				size_t sz;
				const char* cc;
			};

			constexpr stru strue {"lighthouse"};

			constexpr auto test1 = strlen1("asdasdasd");


			template <stru start>
			consteval auto text_to_hex()
			{
				std::array<std::byte, start.sz> hex {};

				for (size_t i = 0; i < start.sz; i++)
					hex[i] = std::byte {int(start.cc[i])};

				return hex;
			}

			constexpr auto test = text_to_hex<stru {"lighthouse"}>();

			enum class data_type
			{

			};

			// beginning of file
			struct header
			{
				using version_t = std::byte[4];

				magic_number_t m_file_start {file_start};
				version_t m_version {};
				checksum_t m_header_checksum {};
				data_size_t m_data_offset {std::byte {sizeof header}};
			};

			// data
			struct data_chunk
			{
				using data_t = data_t;

				data_type m_type;
				data_size_t m_size;
				data_t m_data {};
			};

			// end of file
			struct footer
			{
				checksum_t m_data_checksum;
				magic_number_t m_file_end {file_start};
			};
		};
	}
}