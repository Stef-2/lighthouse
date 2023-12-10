module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module image_view;

import raii_wrapper;
import logical_device;
import image;

export namespace lh
{
	namespace vulkan
	{
		class image_view : public raii_wrapper<vk::raii::ImageView>
		{
		public:
			using raii_wrapper::raii_wrapper;

			struct create_info
			{
				vk::ImageViewCreateInfo m_create_info = {
					{}, {}, vk::ImageViewType::e2D, image::create_info::m_format, {}, default_image_subresource_range()};
			};

			static inline constexpr auto cubemap_create_info = create_info {
				.m_create_info = {{},
				 {},
				 vk::ImageViewType::eCube,
				 image::create_info::m_format,
				 {},
				 {vk::ImageAspectFlagBits::eColor, 0, vk::RemainingMipLevels, 0, 6}}};

			static constexpr auto default_image_subresource_range() -> const vk::ImageSubresourceRange;
			static inline constexpr auto default_image_subresource_layers() -> const vk::ImageSubresourceLayers;
			/*{
				return {vk::ImageAspectFlagBits::eColor, 0, 0, 1};
			}*/

			image_view();
			image_view(const logical_device&, const image&, const create_info& = {});

			auto create_information() const -> const create_info&;

		private:
			create_info m_create_info;
		};
	}
}