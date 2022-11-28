#pragma once

#include "static.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "input.hpp"

namespace lh
{
	struct vulkan_version;

	struct engine_version
	{
		using version_t = uint8_t;
		using packed_version_t = uint32_t;

		engine_version(version_t major, version_t minor, version_t patch);
		// extract the 3 8-bit versions from a packed 32-bit one
		engine_version(packed_version_t);

		// implicit conversions to wider type, packs all 3 versions
		operator packed_version_t();
		// implicit conversion to a readable string format
		operator std::string();

		version_t m_major {};
		version_t m_minor {};
		version_t m_patch {};
	};

	// static engine system
	// provides initialization and main loop facilities
	class engine
	{
	public:
		// one time engine initialization
		static auto initialize(window&, const vulkan_version&) -> void;
		// main engine loop
		static auto run() -> void;

		static auto get_window() -> window&;
		static auto get_version() -> engine_version&;

	private:
		static inline window* m_window = {};
		static inline auto m_version = engine_version {0, 2, 0};

		//renderer m_renderer;
	};
}