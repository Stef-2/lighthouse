module;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/sampler.ixx"
#else
module sampler;
#endif

lh::vulkan::sampler::sampler(const logical_device& logical_device, const create_info& create_info)
{
	const auto sampler_info = vk::SamplerCreateInfo {{},
													 create_info.m_filter,
													 create_info.m_filter,
													 create_info.m_mipmap_mode,
													 create_info.m_address_mode,
													 create_info.m_address_mode,
													 create_info.m_address_mode,
													 {},
													 create_info.m_using_anisotropy,
													 create_info.m_anisotropy,
													 {},
													 vk::CompareOp::eNever,
													 {},
													 {},
													 create_info.m_border_color};

	m_object = {*logical_device, sampler_info};
}
