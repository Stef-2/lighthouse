module;

export module vulkan_utility;

import data_type;

import std;
import vulkan_hpp;

export namespace lh
{
	namespace vulkan
	{
		namespace utility
		{

			template <typename T>
				requires std::integral<T>
			auto aligned_size(T value, T alignment)
			{
				return (value + alignment - 1) & ~(alignment - 1);
			}

			auto shader_object_binary_data(const vk::raii::Device& logical_device, const vk::ShaderEXT shader_object)
			{
				auto data = data_t {};
				auto size = std::size_t {};

				const auto dispatcher = logical_device.getDispatcher();

				dispatcher->vkGetShaderBinaryDataEXT(static_cast<VkDevice>(*logical_device), static_cast<VkShaderEXT>(shader_object), &size, nullptr);

				data.resize(size);

				dispatcher->vkGetShaderBinaryDataEXT(static_cast<VkDevice>(*logical_device),
													 static_cast<VkShaderEXT>(shader_object),
													 &size,
													 data.data());

				return data;
			}

		}
	}
}
