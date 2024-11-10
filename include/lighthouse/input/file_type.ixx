module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif

export module file_type;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import lighthouse_utility;

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
		spir_v,
		shader_binary,
		shader_reflection_data
	};

	const inline auto s_valid_file_extensions = std::map<file_type, const std::vector<const char*>> {
		{file_type::text, {".txt", ".vert", ".frag", ".comp", ".glsl", ".h", ".hpp"}},
		{file_type::image, {".png"}},
		{file_type::font, {".ttf"}},
		{file_type::glsl, {".glsl", ".vert", "frag", ".comp"}},
		{file_type::spir_v, {".spv"}},
		{file_type::shader_binary, {".sbin"}},
		{file_type::shader_reflection_data, {".srd"}}};

	const inline auto s_shader_stage_extensions =
		std::map<const char*, const vk::ShaderStageFlagBits> {{".vert", vk::ShaderStageFlagBits::eVertex},
															  {".frag", vk::ShaderStageFlagBits::eFragment},
															  {".comp", vk::ShaderStageFlagBits::eCompute}};
}
