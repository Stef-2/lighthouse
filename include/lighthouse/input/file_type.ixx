module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif

export module file_type;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import std;

export namespace lh
{
	enum class file_type
	{
		text,
		binary,
		image,
		font,
		scene,
		glsl,
		shader_binary,
		spir_v
	};

	const inline auto s_valid_file_extensions = std::map<file_type, const std::vector<const char*>> {
		{file_type::text, {".txt", ".vert", ".frag", ".glsl", ".h", ".hpp"}},
		{file_type::image, {".png"}},
		{file_type::font, {".ttf"}}};

	const inline auto s_shader_stage_extensions =
		std::map<const char*, const vk::ShaderStageFlagBits> {{"vert", vk::ShaderStageFlagBits::eVertex},
																		 {"frag", vk::ShaderStageFlagBits::eFragment}};
}
