module image;

namespace lh
{
	namespace vulkan
	{
		const vk::ImageSubresourceRange image::s_default_subresource_range = {
			vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

		const vk::ImageSubresourceLayers image::s_default_subresource_layers = {vk::ImageAspectFlagBits::eColor,
																				0,
																				0,
																				1};

		image::image(std::nullptr_t) : m_create_info {}, m_view {nullptr}, m_allocation_info {}, m_allocation {} {}

		image::image(const vulkan::logical_device& logical_device,
					 const vulkan::memory_allocator& memory_allocator,
					 const vk::Extent2D& extent,
					 const create_info& create_info)
			: m_create_info {create_info}, m_view {nullptr}, m_allocation_info {}, m_allocation {}
		{
			const auto image_info = vk::ImageCreateInfo {create_info.m_image_create_flags,
														 create_info.m_image_type,
														 create_info.m_format,
														 vk::Extent3D(extent, 1),
														 create_info.m_mip_levels,
														 1,
														 create_info.m_image_sample_count,
														 create_info.m_image_tiling,
														 create_info.m_image_usage,
														 create_info.m_image_sharing_mode,
														 {}};

			const auto allocation_create_info = vma::AllocationCreateInfo {{}, vma::MemoryUsage::eAuto};
			auto allocation_info = vma::AllocationInfo {};

			auto [image,
				  allocation] = memory_allocator->createImage(image_info, allocation_create_info, allocation_info);

			m_object = {*logical_device, image};

			const auto view_info = vk::ImageViewCreateInfo {{},
															*m_object,
															create_info.m_view_type,
															create_info.m_format,
															{},
															{create_info.m_image_aspect, 0, 1, 0, 1}};

			m_view = {*logical_device, view_info};
		}

		auto image::default_subresource_range() -> const vk::ImageSubresourceRange&
		{
			return s_default_subresource_range;
		}

		auto image::default_subresource_layers() -> const vk::ImageSubresourceLayers&
		{
			return s_default_subresource_layers;
		}

		auto image::create_information() const -> const image::create_info&
		{
			return m_create_info;
		}

		auto image::view() const -> const vk::raii::ImageView&
		{
			return m_view;
		}

		auto image::transition_layout(const vk::raii::CommandBuffer& command_buffer,
									  const layout_transition_data& transition_data) -> void
		{
			const auto barrier = vk::ImageMemoryBarrier2 {transition_data.m_source_pipeline_stage,
														  transition_data.m_source_access_flags,
														  transition_data.m_destination_pipeline_stage,
														  transition_data.m_destination_access_flags,
														  transition_data.m_old_layout,
														  transition_data.m_new_layout,
														  transition_data.m_source_queue_family,
														  transition_data.m_destination_queue_family,
														  *m_object,
														  transition_data.m_subresource_range};

			const auto dependency_info = vk::DependencyInfo {{}, {}, {}, barrier};

			command_buffer.pipelineBarrier2(dependency_info);
		}

		auto image::allocation_info() const -> const vma::AllocationInfo&
		{
			return m_allocation_info;
		}
		auto image::allocation_info() -> vma::AllocationInfo&
		{
			return m_allocation_info;
		}
		auto image::allocation() const -> const vma::Allocation&
		{
			return m_allocation;
		}
	}
}
