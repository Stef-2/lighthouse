#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class logical_device;

		class sampler : vk_wrapper<vk::raii::Sampler>
		{
		public:
			struct create_info
			{
				vk::Filter m_filter = vk::Filter::eNearest;
				vk::SamplerMipmapMode m_mipmap_mode = vk::SamplerMipmapMode::eNearest;
				vk::SamplerAddressMode m_address_mode = vk::SamplerAddressMode::eClampToBorder;
				vk::BorderColor m_border_color = vk::BorderColor::eFloatOpaqueBlack;
				decltype(vk::SamplerCreateInfo::anisotropyEnable) m_using_anisotropy = {true};
				decltype(vk::SamplerCreateInfo::maxAnisotropy) m_anisotropy = 16.0f;
			};

			sampler(const logical_device&, const create_info& = {});

		private:
		};
	}
};
