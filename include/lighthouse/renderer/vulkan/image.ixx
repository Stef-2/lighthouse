module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module image;

import raii_wrapper;
import logical_device;
import memory_allocator;

export namespace lh
{
	namespace vulkan
	{
		class image : public raii_wrapper<vk::raii::Image>
		{
		public:
			using raii_wrapper::raii_wrapper;

			struct create_info
			{
				static inline constexpr auto m_format = vk::Format::eR8G8B8A8Srgb;

				vk::ImageCreateInfo m_image_create_info = {{},
														   vk::ImageType::e2D,
														   m_format,
														   vk::Extent3D{},
														   1,
														   1,
														   vk::SampleCountFlagBits::e1,
														   vk::ImageTiling::eOptimal,
														   vk::ImageUsageFlagBits::eSampled,
														   vk::SharingMode::eExclusive};

				vk::MemoryPropertyFlagBits m_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;

				vma::AllocationCreateInfo m_allocation_create_info = {{},
																	  vma::MemoryUsage::eAutoPreferDevice,
																	  m_memory_properties,
																	  m_memory_properties};
			};

			static inline constexpr auto cubemap_create_info = create_info {.m_image_create_info = {vk::ImageCreateFlagBits::eCubeCompatible,
																	   vk::ImageType::e2D,
																	   image::create_info::m_format,
																	   vk::Extent3D {},
																	   1,
																	   6,
																	   vk::SampleCountFlagBits::e1,
																	   vk::ImageTiling::eOptimal,
																	   vk::ImageUsageFlagBits::eSampled,
																	   vk::SharingMode::eExclusive}};
			
			struct layout_transition_data
			{
				vk::PipelineStageFlags2 m_source_pipeline_stage = vk::PipelineStageFlagBits2::eTopOfPipe;
				vk::PipelineStageFlags2 m_destination_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer;
				vk::AccessFlags2 m_source_access_flags = {};
				vk::AccessFlags2 m_destination_access_flags = vk::AccessFlagBits2::eTransferWrite;
				vk::ImageLayout m_old_layout = vk::ImageLayout::eUndefined;
				vk::ImageLayout m_new_layout = vk::ImageLayout::eTransferDstOptimal;
				std::uint32_t m_source_queue_family = vk::QueueFamilyIgnored;
				std::uint32_t m_destination_queue_family = vk::QueueFamilyIgnored;
				vk::ImageSubresourceRange m_subresource_range = {
					vk::ImageAspectFlagBits::eColor, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers};
			};

			image(nullptr_t);
			image(const vulkan::logical_device&,
				  const vulkan::memory_allocator&,
				  const create_info& = {});

			auto transition_layout(const vk::raii::CommandBuffer&, const layout_transition_data& = {}) -> void;

			auto create_information() const -> const image::create_info&;
			auto allocation_info() const -> const vma::AllocationInfo&;
			auto allocation_info() -> vma::AllocationInfo&;
			auto allocation() const -> const vma::Allocation&;

		private:
			create_info m_create_info;

			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;
		};
	}
}