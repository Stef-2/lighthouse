#include "lighthouse/vulkan/logical_device.hpp"
#include "lighthouse/vulkan/shader_object.hpp"
#include "lighthouse/vulkan/descriptor_set_layout.hpp"
#include "lighthouse/vulkan/spir_v.hpp"

lh::vulkan::shader_object::shader_object(const logical_device& logical_device,
										 const spir_v& spir_v,
										 const descriptor_set_layout& descriptor_set_layout,
										 const create_info& create_info)
{

	const auto shader_create_info = vk::ShaderCreateInfoEXT {create_info.m_flags,
															 spir_v.stage(),
															 {},
															 create_info.m_code_type,
															 spir_v.code().size(),
															 spir_v.code().data(),
															 "main",
															 1,
															 &(**descriptor_set_layout)};

	m_object = {*logical_device, shader_create_info};
}

lh::vulkan::shader_objects::shader_objects(
	const logical_device& logical_device,
	const std::vector<std::pair<const spir_v&, const descriptor_set_layout&>> shaders,
	const create_info& create_info)

{
	auto shaders_create_info = std::vector<vk::ShaderCreateInfoEXT> {shaders.size()};

	for (std::size_t i {0}; i < shaders.size(); i++)
		shaders_create_info.push_back({create_info.m_flags,
									   shaders[i].first.stage(),
									   {},
									   create_info.m_code_type,
									   shaders[i].first.code().size(),
									   shaders[i].first.code().data(),
									   "main",
									   1,
									   &(**shaders[i].second)});

	m_object = vk::raii::ShaderEXTs {*logical_device, shaders_create_info};
}
