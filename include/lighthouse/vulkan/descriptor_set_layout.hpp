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
				decltype(vk::DescriptorSetLayoutBinding::binding) m_location {};
				vk::DescriptorType m_type {};
				decltype(vk::DescriptorSetLayoutBinding::descriptorCount) m_count {1};
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
