#include "lighthouse/renderer/vulkan/extension.hpp"

lh::vulkan::logical_extensions::logical_extensions(const vk_extensions_t& supported, const create_info& create_info)
	: vulkan_extension_module(supported, create_info)
{}

auto lh::vulkan::logical_extensions::extension_type() const -> string::string_t
{
	return "logical extension";
}

lh::vulkan::physical_extensions::physical_extensions(const vk_extensions_t& supported, const create_info& create_info)
	: vulkan_extension_module(supported, create_info)
{}

auto lh::vulkan::physical_extensions::extension_type() const -> string::string_t
{
	return "physical extension";
}

lh::vulkan::validation_layers::validation_layers(const vk_layers_t& supported, const create_info& create_info)
	: vulkan_extension_module(supported, create_info)
{}

auto lh::vulkan::validation_layers::extension_type() const -> string::string_t
{
	return "validation layer";
}
