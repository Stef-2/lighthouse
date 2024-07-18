module;

#if INTELLISENSE
	#include <cstddef>
	#include <vector>
	#include <array>
#endif

export module lhd_format;

import std;

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