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
				vk::ImageViewCreateInfo m_create_info = {};
			};

			static constexpr auto default_image_subresource_range() -> const vk::ImageSubresourceRange;
			static constexpr auto default_image_subresource_layers() -> const vk::ImageSubresourceLayers;

			image_view();
			image_view(const logical_device&, const image&, const create_info& = {});

		private:
		};
	}
}