module;

module sampler;

namespace lh
{
	namespace vulkan
	{
		sampler::sampler(const logical_device& logical_device, const create_info& create_info)
		{
			m_object = {*logical_device, create_info.m_sampler_create_info};
		}
	}
}
