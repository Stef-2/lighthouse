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
		shader_reflection_data,
		shader_binary
	};

	const inline auto s_valid_file_extensions = std::map<file_type, const std::vector<const char*>> {
		{file_type::text, {".txt", ".vert", ".frag", ".comp", ".glsl", ".h", ".hpp"}},
		{file_type::image, {".png"}},
		{file_type::font, {".ttf"}},
		{file_type::glsl, {".glsl", ".vert", "frag", ".comp"}},
		{file_type::spir_v, {".spv"}},
		{file_type::shader_reflection_data, {".srd"}},
		{file_type::shader_binary, {".sbin"}}};

	// shader files are identified and parsed based on an arbitrary combination of their file extensions
	// e.g:
	// shader.vert -> glsl vertex shader
	// shader.vert.spv -> precompiled spir_v binary of the shader above
	// shader.vert.srd -> shader reflection data for the shader above
	// shader.vert.sbin -> precompiled shader object for the shader above
	consteval auto shader_stage_file_extension(const vk::ShaderStageFlagBits stage)
	{
		switch (stage)
		{
			case vk::ShaderStageFlagBits::eVertex: return "vert";
			case vk::ShaderStageFlagBits::eFragment: return "frag";
			case vk::ShaderStageFlagBits::eCompute: return "comp";
		}
	}
}
