module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module sampler;

import raii_wrapper;
import logical_device;

export namespace lh
{
	namespace vulkan
	{
		class sampler : public raii_wrapper<vk::raii::Sampler>
		{
		public:
			using raii_wrapper::raii_wrapper;

			struct create_info
			{
				vk::SamplerCreateInfo m_create_info = {{},
																		vk::Filter::eNearest,
																		vk::Filter::eNearest,
																		vk::SamplerMipmapMode::eNearest,
																		vk::SamplerAddressMode::eRepeat,
																		vk::SamplerAddressMode::eRepeat,
																		vk::SamplerAddressMode::eRepeat,
																		0.0f,
																		false,
																		0.0f,
																		false,
																		vk::CompareOp::eNever,
																		0.0f,
																		0.0f,
																		vk::BorderColor::eFloatTransparentBlack,
																		false};
			};

			sampler(const logical_device&, const create_info& = {});

		private:
		};
	}
};
