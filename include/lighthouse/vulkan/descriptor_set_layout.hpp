#pragma once

#include "lighthouse/vulkan/raii_wrapper.hpp"

#include <vector>

namespace lh
{
	namespace vulkan
	{

		// forward declarations
		class logical_device;

		class descriptor_set_layout : public vk_wrapper<vk::raii::DescriptorSetLayout>
		{
		public:
			struct binding
			{
				using descriptor_count_t = uint32_t;
				using binding_location_t = uint32_t;

				binding_location_t m_location {};
				vk::DescriptorType m_type {};
				descriptor_count_t m_count {1};
			};

			struct create_info
			{
				vk::DescriptorSetLayoutCreateFlags m_flags {};
				vk::ShaderStageFlagBits m_access = vk::ShaderStageFlagBits::eAll;
				std::vector<binding> m_bindings {};
			};

			descriptor_set_layout(const logical_device&, const create_info& = {});

		private:
		};
	}
}
