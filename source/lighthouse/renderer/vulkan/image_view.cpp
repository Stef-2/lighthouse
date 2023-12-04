module;

module image_view;

namespace lh
{
	namespace vulkan
	{
		constexpr auto image_view::default_image_subresource_range() -> const vk::ImageSubresourceRange
		{
			return {vk::ImageAspectFlagBits::eColor, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers};
		}

		constexpr auto image_view::default_image_subresource_layers() -> const vk::ImageSubresourceLayers
		{
			return {vk::ImageAspectFlagBits::eColor, 0, 0, 1};
		}
	}
}
