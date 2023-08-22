module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#endif

export module descriptor_set_layout;

import raii_wrapper;
import logical_device;

#if not INTELLISENSE
import std.core;
#endif

export namespace lh
{
	namespace vulkan
	{
		class descriptor_set_layout : public raii_wrapper<vk::raii::DescriptorSetLayout>
		{
		public:
			using raii_wrapper::raii_wrapper;

			struct binding
			{
				decltype(vk::DescriptorSetLayoutBinding::binding) m_binding {};
				vk::DescriptorType m_type {};
				decltype(vk::DescriptorSetLayoutBinding::descriptorCount) m_count {1};
			};

			struct create_info
			{
				vk::DescriptorSetLayoutCreateFlags m_modifier_flags {
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
