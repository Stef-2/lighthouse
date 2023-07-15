module;
#pragma once

#include <vector>

export module descriptor_set_layout;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/raii_wrapper.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#else
import raii_wrapper;
import logical_device;
#endif

export namespace lh
{
	namespace vulkan
	{
		class descriptor_set_layout : public raii_wrapper<vk::raii::DescriptorSetLayout>
		{
		public:
			// using vk_wrapper::vk_wrapper;

			struct binding
			{
				decltype(vk::DescriptorSetLayoutBinding::binding) m_binding {};
				vk::DescriptorType m_type {};
				decltype(vk::DescriptorSetLayoutBinding::descriptorCount) m_count {1};
			};

			struct create_info
			{
				vk::DescriptorSetLayoutCreateFlags m_flags {
					vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT};
				vk::ShaderStageFlagBits m_access = vk::ShaderStageFlagBits::eAll;
			};

			descriptor_set_layout(const logical_device&, const std::vector<binding>&, const create_info& = {});

			auto bindings() const -> const std::vector<binding>&;

		private:
			std::vector<binding> m_bindings;
		};
	}
}
