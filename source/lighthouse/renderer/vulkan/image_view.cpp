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

		image_view::image_view() : raii_wrapper {nullptr} {}

		image_view::image_view(const logical_device& logical_device, const image& image, const create_info& create_info)
			: raii_wrapper {nullptr}, m_create_info {create_info}
		{
			auto view_create_info = create_info.m_create_info;
			view_create_info.image = **image;
			m_object = {logical_device, view_create_info};
		}

		auto image_view::create_information() const -> const create_info&
		{
			return m_create_info;
		}
	}
}
